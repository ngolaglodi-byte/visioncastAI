using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
namespace MWModle
{
    public class CMWCaptureQuad
    {
        public struct frame_info_t
        {
            public long time;
            public int frame_index;
            public Int32 index;
        }
        public class CFrameCompare : IComparer<frame_info_t>
        {
            public int Compare(frame_info_t x, frame_info_t y)
            {
                if (x.time > y.time)
                    return 1;
                else if (x.time < y.time)
                    return -1;
                else
                    return 0;
            }
        }
        public struct device_info_t
        {
            public int capture_frame_index;
            public int prev_notfiy_index;
            public int max_device_buffer_num;
            public long prev_notfiy_time;
            public long max_cover_time;
            public Int32 device_index;
            public IntPtr channel_handle;
            public IntPtr notify_event;
            public IntPtr capture_event;
            public UInt64 notify;
            public LibMWCapture.RECT[] p_rect_src;
        }


        public CRingBuffer m_video_buffer = null;
        public Boolean m_runing = false;

        public Int64 m_frame_duration = 0;
        public Int32 m_capture_width = 0;
        public Int32 m_capture_height = 0;
        public Int32 m_frame_width = 0;
        public Int32 m_frame_height = 0;
        public UInt32 m_frame_stride = 0;
        public UInt32 m_frame_size = 0;
        public Boolean m_sync_mode = false;
        public UInt32 m_mw_fourcc = MWFOURCC.MWFOURCC_NV12;

        device_info_t[] m_device_info = null;
        protected Int32 m_device_num = 0;

        Thread m_capture_thread = null;
        Thread m_sync_thread = null;

        protected UInt32 m_now_capture_frames = 0;
        UInt32 m_prev_capture_frames = 0;
        UInt32 m_prev_ts = 0;
        Double m_video_capture_fps = 0.0;
        int m_sync_mask = 0;
        long m_prev_set_time = 0;
        List<frame_info_t> m_frame_info_list = new List<frame_info_t>();
        IComparer<frame_info_t> comparer = new CFrameCompare();
        public CMWCaptureQuad()
        {
        }
        public virtual void Dispose()
        {
            if (m_runing){
                m_runing = false;
                if (m_capture_thread != null){
                    m_capture_thread.Join();
                    m_capture_thread = null;
                }
                if (m_sync_thread != null){
                    m_sync_thread.Join();
                    m_sync_thread = null;
                }
            }
            if (null != m_video_buffer) {
                m_video_buffer.Dispose();
                m_video_buffer = null;
            }
            if ((null != m_device_info) && (m_device_num > 0)){
                for (int i = 0; i < m_device_num; i++) {
                    if (IntPtr.Zero != m_device_info[i].notify_event){
                        Libkernel32.CloseHandle(m_device_info[i].notify_event);
                    }
                    if (IntPtr.Zero != m_device_info[i].capture_event){
                        Libkernel32.CloseHandle(m_device_info[i].capture_event);
                    }
                    if (0 != m_device_info[i].notify){
                        LibMWCapture.MWUnregisterNotify(m_device_info[i].channel_handle, m_device_info[i].notify);
                    }

                    if (IntPtr.Zero != m_device_info[i].channel_handle){
                        LibMWCapture.MWCloseChannel(m_device_info[i].channel_handle);
                        m_device_info[i].channel_handle = IntPtr.Zero;
                    }
                    m_device_info[i].p_rect_src = null;
                }
            }
            m_device_info = null;
            m_device_num = 0;
        }

        ~CMWCaptureQuad()
        {
            Dispose();
        }

        static public void Init()
        {
            LibMWCapture.MWCaptureInitInstance();
        }

        static public void Exit()
        {
            LibMWCapture.MWCaptureExitInstance();
        }

        static public Boolean RefreshDevices()
        {
            LibMWCapture.MW_RESULT mr;
            mr = LibMWCapture.MWRefreshDevice();
            if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                return false;
            }
            return true;
        }

        static public int GetChannelCount()
        {
            return LibMWCapture.MWGetChannelCount();
        }

        static public bool GetChannelInfobyIndex(int index, ref LibMWCapture.MWCAP_CHANNEL_INFO channel_info)
        {
            if (LibMWCapture.MW_RESULT.MW_SUCCEEDED != LibMWCapture.MWGetChannelInfoByIndex(index, ref channel_info)) {
                return false;
            }
            return true;
        }
        Boolean set_device(int[] magewell_device_index, Int32 device_num)
        {
            m_device_num = device_num;
            m_device_info = new device_info_t[device_num];
            for (int i = 0; i < device_num; i++){
                UInt16[] wpath = new UInt16[512];
                //IntPtr pwpath = GCHandle.Alloc(wpath, GCHandleType.Pinned).AddrOfPinnedObject();
                LibMWCapture.MWGetDevicePath(magewell_device_index[i], wpath);
                m_device_info[i].channel_handle = LibMWCapture.MWOpenChannelByPath(wpath);
                if (IntPtr.Zero == m_device_info[i].channel_handle) {
                    Console.WriteLine("open channel fail "+ magewell_device_index[i]);
                    return false;
                }
                m_device_info[i].prev_notfiy_index = -1;
                m_device_info[i].device_index = magewell_device_index[i];
                m_device_info[i].max_device_buffer_num = 4;
                m_device_info[i].max_cover_time = 10000000/60*2;
                m_device_info[i].capture_frame_index = -1;
                m_device_info[i].notify_event = IntPtr.Zero;
                m_device_info[i].capture_event = IntPtr.Zero;
                m_device_info[i].notify = 0;
                m_device_info[i].p_rect_src = null;
            }
            return true;
        }
        protected Boolean create_video_buffer(int width, int height, Int32 layout_index, Int32 device_num)
        {
            m_capture_width = width;
            m_capture_height = height;
            if (layout_index == 0) {
                m_frame_width = width * 2;
                m_frame_height = height * 2;
                m_device_info[0].p_rect_src = new LibMWCapture.RECT[1];
                m_device_info[0].p_rect_src[0].left = 0;
                m_device_info[0].p_rect_src[0].top = 0; 
                m_device_info[0].p_rect_src[0].right = width; 
                m_device_info[0].p_rect_src[0].bottom = height;
                if (device_num >= 2){
                    m_device_info[1].p_rect_src = new LibMWCapture.RECT[1];
                    m_device_info[1].p_rect_src[0].left = width;
                    m_device_info[1].p_rect_src[0].top = 0;
                    m_device_info[1].p_rect_src[0].right = 2 * width;
                    m_device_info[1].p_rect_src[0].bottom = height;
                }
                if (device_num >= 3) {
                    m_device_info[2].p_rect_src = new LibMWCapture.RECT[1];
                    m_device_info[2].p_rect_src[0].left = 0;
                    m_device_info[2].p_rect_src[0].top = height;
                    m_device_info[2].p_rect_src[0].right = width;
                    m_device_info[2].p_rect_src[0].bottom = 2 * height;
                }
                if (device_num >= 4){
                    m_device_info[3].p_rect_src = new LibMWCapture.RECT[1];
                    m_device_info[3].p_rect_src[0].left = width;
                    m_device_info[3].p_rect_src[0].top = height;
                    m_device_info[3].p_rect_src[0].right = 2 * width;
                    m_device_info[3].p_rect_src[0].bottom = 2 * height;
                }
            } else if (layout_index == 1) {
                m_frame_width = width * 2;
                m_frame_height = height;
                m_device_info[0].p_rect_src = new LibMWCapture.RECT[1];
                m_device_info[0].p_rect_src[0].left = 0;
                m_device_info[0].p_rect_src[0].top = 0;
                m_device_info[0].p_rect_src[0].right = width;
                m_device_info[0].p_rect_src[0].bottom = height;
                if (device_num > 1) {
                    m_device_info[1].p_rect_src = new LibMWCapture.RECT[1];
                    m_device_info[1].p_rect_src[0].left = width;
                    m_device_info[1].p_rect_src[0].top = 0;
                    m_device_info[1].p_rect_src[0].right = 2*width;
                    m_device_info[1].p_rect_src[0].bottom = height;
                }
            } else {
                m_frame_width = width;
                m_frame_height = height;
                m_device_info[0].p_rect_src = null;
            }

            m_frame_stride = MWFOURCC.FOURCC_CalcMinStride(m_mw_fourcc, m_frame_width, 2);
            m_frame_size = MWFOURCC.FOURCC_CalcImageSize(m_mw_fourcc, m_frame_width, m_frame_height, m_frame_stride);
            m_video_buffer = new CRingBuffer();
            if (null == m_video_buffer){
                return false;
            }
            return m_video_buffer.set_property(5, m_frame_size);
        }
        public bool start_capture(int[] magewell_device_index, Int32 device_num, Int32 layout_index, int width, int height, double fps, bool sync_mode)
        {
            if (!set_device(magewell_device_index, device_num)) {
                return false;
            }
            if (!create_video_buffer(width, height, layout_index, device_num)){
                return false;
            }
            m_sync_mode = sync_mode;
            m_frame_duration = (long)(10000000 / fps);
            m_runing = true;
            m_capture_thread = new Thread(new ThreadStart(video_capture));
            if (null == m_capture_thread){
                Console.WriteLine("capture video fail");
                return false;
            }
            m_capture_thread.Start();
            return true;
        }
        void reset_time() {
            Int64 time = 0;
            for (int i = 0; i < m_device_num; i++){
                if (0 == time){
                    LibMWCapture.MWGetDeviceTime(m_device_info[i].channel_handle, ref time);
                }
                else{
                    LibMWCapture.MWRegulateDeviceTime(m_device_info[i].channel_handle, time);
                }
            }
        }
        void frame_list_add(int index, int frame_index, long time)
        {

            frame_info_t frame_info_node = new frame_info_t();
            frame_info_node.time = time;
            frame_info_node.frame_index = frame_index;
            frame_info_node.index = index;
            lock (m_frame_info_list){
                m_frame_info_list.Add(frame_info_node);
            }

        }

        bool sync_frame_list(long now_time)
        {
            lock (m_frame_info_list)
            {
                int size = m_frame_info_list.Count;
                m_frame_info_list.Sort(0, size, comparer);
                for (int i = size - 1; i >= 0; i--){
                    int now_mask = 0;
                    long new_time = m_frame_info_list[i].time;
                    for (int j = i; j >= 0; j--){
                        if ((new_time - m_frame_info_list[j].time) > m_frame_duration / 10){
                            break;
                        }

                        if ((now_time - m_frame_info_list[j].time) >= m_device_info[m_frame_info_list[j].index].max_cover_time){//frame cover
//                             Console.WriteLine("out time " + size + " " + m_frame_info_list.Count + " " + j + " " + now_time);
//                             for (int kk = 0; kk < m_frame_info_list.Count; kk++)
//                             {
//                                 Console.WriteLine(m_frame_info_list.Count + "out" + m_frame_info_list[kk].index + " " + m_frame_info_list[kk].frame_index + " " + (m_frame_info_list[kk].time / 10000) % 10000);
//                             }
                            m_frame_info_list.RemoveRange(0, j + 1);
                            return false;
                        }
                        if ((now_mask & (1 << m_frame_info_list[j].index)) == 0){
                            m_device_info[m_frame_info_list[j].index].capture_frame_index = m_frame_info_list[j].frame_index;
                            now_mask |= 1 << m_frame_info_list[j].index;
                            if (now_mask >= m_sync_mask){
                                m_frame_info_list.RemoveRange(0, i + 1);
                                return true;
                            }
                        }
                    }
                }
//                 Console.WriteLine("not find " + size + " " + m_frame_info_list.Count);
//                 for (int kk = 0; kk < m_frame_info_list.Count; kk++)
//                 {
//                     Console.WriteLine(m_frame_info_list.Count + "not" + m_frame_info_list[kk].index + " " + m_frame_info_list[kk].frame_index + " " + (m_frame_info_list[kk].time / 10000) % 10000);
//                 }
            }
            return false;
        }
        void signal_process()
        {
            IntPtr[] event_list = new IntPtr[m_device_num];
            LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS video_signal_status = new LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS();
            LibMWCapture.MWCAP_VIDEO_BUFFER_INFO buffer_info = new LibMWCapture.MWCAP_VIDEO_BUFFER_INFO();
            LibMWCapture.MWCAP_VIDEO_FRAME_INFO frame_info = new LibMWCapture.MWCAP_VIDEO_FRAME_INFO();
            for (int i = 0; i < m_device_num; i++){
                m_device_info[i].notify_event = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
                if (IntPtr.Zero == m_device_info[i].notify_event){
                    Console.WriteLine("create notify event fail " + i);
                    return;
                }
                event_list[i] = m_device_info[i].notify_event;
                m_device_info[i].notify = LibMWCapture.MWRegisterNotify(m_device_info[i].channel_handle, m_device_info[i].notify_event,
                    LibMWCapture.MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED | LibMWCapture.MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE);
                if (0 == m_device_info[i].notify){
                    Console.WriteLine("register notify fail " + i);
                    return;
                }
                if (LibMWCapture.MWGetVideoBufferInfo(m_device_info[i].channel_handle, ref buffer_info) == LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    m_device_info[i].max_device_buffer_num = (int)buffer_info.cMaxFrames;
                }
                if (LibMWCapture.MWGetVideoSignalStatus(m_device_info[i].channel_handle, ref video_signal_status) != LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    Console.WriteLine("MWGetVideoSignalStatus fail " + i);
                    continue;
                }

                if (video_signal_status.state != LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED){
                    Console.WriteLine("check input" + i);
                    continue;
                }
                m_device_info[i].max_cover_time = video_signal_status.dwFrameDuration * ((buffer_info.cMaxFrames > 2) ? (buffer_info.cMaxFrames - 1):1);
                m_sync_mask |= (1 << i);
            }
            reset_time();
            while (m_runing) {
                uint ret = Libkernel32.WaitForMultipleObjects((uint)m_device_num, event_list, 0, 100);
                if (258 == ret){//WAIT_TIMEOUT
                    continue;
                }
                uint index = ret - 0;//WAIT_OBJECT_0
                if (index > m_device_num){
                    Console.WriteLine("WaitForMultipleObjects fail");
                    break;
                }
                ulong status = 0;
                if (LibMWCapture.MWGetNotifyStatus(m_device_info[index].channel_handle, m_device_info[index].notify, ref status) != 
                    LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    continue;
                }
                if (0 != (status & LibMWCapture.MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE)) {
                    if (LibMWCapture.MWGetVideoSignalStatus(m_device_info[index].channel_handle, ref video_signal_status) != 
                        LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                        m_device_info[index].max_cover_time = video_signal_status.dwFrameDuration * 
                            ((m_device_info[index].max_device_buffer_num > 2) ? (m_device_info[index].max_device_buffer_num - 1) : 1);
                        if (video_signal_status.state == LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED){
                            m_sync_mask |= (1 << (int)index);
                        }
                        else{
                            m_sync_mask &= ~(1 << (int)index);
                        } 
                    }
                }
                if (0 == (status & LibMWCapture.MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED)) {
                    continue;
                }

                if (LibMWCapture.MWGetVideoBufferInfo(m_device_info[index].channel_handle, ref buffer_info) != LibMWCapture.MW_RESULT.MW_SUCCEEDED) {
                    continue;
                }

                if (LibMWCapture.MWGetVideoFrameInfo(m_device_info[index].channel_handle, buffer_info.iNewestBuffered, ref frame_info) != 
                    LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    continue;
                }
                int buffered_num = (buffer_info.iNewestBuffered + m_device_info[index].max_device_buffer_num - m_device_info[index].prev_notfiy_index) % m_device_info[index].max_device_buffer_num;
                long buffered_time = frame_info.allFieldBufferedTimes[0] - m_device_info[index].prev_notfiy_time;
                if ((buffered_num > 1) && (m_device_info[index].prev_notfiy_index>=0) &&(buffered_time < m_device_info[index].max_cover_time)) {
                    long avg = buffered_time / buffered_num;
                    int buffered_index = buffer_info.iNewestBuffered - 1;
                    if (buffered_index < 0) {
                        buffered_index = m_device_info[index].max_device_buffer_num - 1;
                    }
                    //Console.WriteLine("add "+index+ " "+buffered_index+" "+ ((frame_info.allFieldBufferedTimes[0] - avg)/10000)%10000);
                    frame_list_add((int)index, buffered_index, frame_info.allFieldBufferedTimes[0] - avg);
                    if (buffered_num > 2) {
                        buffered_index--;
                        if (buffered_index < 0){
                            buffered_index = m_device_info[index].max_device_buffer_num - 1;
                        }
                        frame_list_add((int)index, buffered_index, frame_info.allFieldBufferedTimes[0] - 2*avg);
                    }
                }
                m_device_info[index].prev_notfiy_index = buffer_info.iNewestBuffered;
                m_device_info[index].prev_notfiy_time = frame_info.allFieldBufferedTimes[0];
                frame_list_add((int)index, buffer_info.iNewestBuffered, frame_info.allFieldBufferedTimes[0]);
            }
        }

        void video_capture() {
            Thread signal_thread = null;
            Console.WriteLine("capture video in\n");
            for (int i = 0; i < m_device_num; i++) {
                m_device_info[i].capture_event = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
                if (IntPtr.Zero == m_device_info[i].capture_event){
                    Console.WriteLine("create capture event fail "+i);
                    return;
                }
                if (LibMWCapture.MWStartVideoCapture(m_device_info[i].channel_handle, m_device_info[i].capture_event) != 
                    LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    Console.WriteLine("start video capture fail "+i);
                    Libkernel32.CloseHandle(m_device_info[i].capture_event);
                    m_device_info[i].capture_event = IntPtr.Zero;
                    return;
                }
            }
            IntPtr timer_event = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
            UInt64 timer = LibMWCapture.MWRegisterTimer(m_device_info[0].channel_handle, timer_event);
            if (timer == 0){
                Console.WriteLine("register notify fail");
                goto end_and_free;
            }
            
            if (m_sync_mode) {
                signal_thread = new Thread(new ThreadStart(signal_process));
                if (null == signal_thread){
                    Console.WriteLine("signal process fail");
                    goto end_and_free;
                }
                signal_thread.Start();
            }

            CRingBuffer.st_frame_t frame = m_video_buffer.get_buffer_by_index(0);
            frame.buffer_len = 0;
            frame.p_buffer = null;
            Int64 wait_tm = 0;
            LibMWCapture.MWGetDeviceTime(m_device_info[0].channel_handle, ref wait_tm);
            IntPtr p_capture_status = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(LibMWCapture.MWCAP_VIDEO_CAPTURE_STATUS)));
            IntPtr[] capture_event = new IntPtr[m_device_num];
            while (m_runing){
                uint event_num = 0;
                wait_tm += m_frame_duration;
                if (LibMWCapture.MWScheduleTimer(m_device_info[0].channel_handle, timer, wait_tm) != LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    continue;
                }
                if (0 != Libkernel32.WaitForSingleObject(timer_event, 1000)){
                    Console.WriteLine("WaitForSingleObject timer_event");
                    continue;
                }

                if (0 == frame.buffer_len){
                    frame = m_video_buffer.get_buffer_to_fill();
                }

                if (0 == frame.buffer_len){
                    continue;
                }
                if (m_sync_mode){
                    long now_time = 0;
                    LibMWCapture.MWGetDeviceTime(m_device_info[0].channel_handle, ref now_time);
                    if (now_time - m_prev_set_time > 20000000) {
                        reset_time();
                        m_prev_set_time = now_time;
                    }
                    if (sync_frame_list(now_time)) {
                        for (int i = 0; i < m_device_num; i++){
                            if ((m_device_info[i].capture_frame_index < 0) || (0 == ((1<< i) & m_sync_mask))) {
                                continue;
                            }
                            LibMWCapture.MWCaptureVideoFrameToVirtualAddressEx(m_device_info[i].channel_handle,
                                m_device_info[i].capture_frame_index, frame.p_buffer, m_frame_size, m_frame_stride,
                                0, 0, m_mw_fourcc, m_frame_width, m_frame_height, 0, 0, 0, null, 0, 100, 0, 100, 0,
                                LibMWCapture.MWCAP_VIDEO_DEINTERLACE_MODE.MWCAP_VIDEO_DEINTERLACE_BLEND,
                                LibMWCapture.MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE.MWCAP_VIDEO_ASPECT_RATIO_IGNORE,
                                null, m_device_info[i].p_rect_src, 0, 0, LibMWCapture.MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
                                LibMWCapture.MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
                                LibMWCapture.MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_UNKNOWN);
                            capture_event[event_num] = m_device_info[i].capture_event;
                            event_num++;
                        }
                    }
                }
                else {
                    for (int i = 0; i < m_device_num; i++){
                        LibMWCapture.MWCaptureVideoFrameToVirtualAddressEx(m_device_info[i].channel_handle,
                        LibMWCapture.MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, frame.p_buffer, m_frame_size, m_frame_stride,
                        0, 0, m_mw_fourcc, m_frame_width, m_frame_height, 0, 0, 0, null, 0, 100, 0, 100, 0,
                        LibMWCapture.MWCAP_VIDEO_DEINTERLACE_MODE.MWCAP_VIDEO_DEINTERLACE_BLEND,
                        LibMWCapture.MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE.MWCAP_VIDEO_ASPECT_RATIO_IGNORE,
                        null, m_device_info[i].p_rect_src, 0, 0, LibMWCapture.MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
                        LibMWCapture.MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
                        LibMWCapture.MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_UNKNOWN);
                        capture_event[event_num] = m_device_info[i].capture_event;
                        event_num++;
                    }
                }
                if (event_num > 0) {
                    if (0 != Libkernel32.WaitForMultipleObjects(event_num, capture_event, 1, 100)){
                        Console.WriteLine("WaitForSingleObject capture_event");
                        continue;
                    }
                    m_video_buffer.buffer_filled(frame);
                    m_now_capture_frames++;
                    frame.buffer_len = 0;
                }
                for (int i = 0; i < m_device_num; i++){
                    LibMWCapture.MWGetVideoCaptureStatus(m_device_info[i].channel_handle, p_capture_status);
                }
            }

            Console.WriteLine("capture video out");
            end_and_free:
            if (null != signal_thread) {
                signal_thread.Join();
                signal_thread = null;
            }
            if (0 != timer){
                LibMWCapture.MWUnregisterTimer(m_device_info[0].channel_handle, timer);
            }
            if (IntPtr.Zero != timer_event){
                Libkernel32.CloseHandle(timer_event);
            }

        }
        public Double get_video_capture_fps()
        {
            UInt32 now_ts = (UInt32)Libkernel32.GetTickCount();
            if (0 == m_prev_ts) {
                m_prev_ts = now_ts;
                m_prev_capture_frames = m_now_capture_frames;
                return m_video_capture_fps;
            }
            UInt32 time = now_ts - m_prev_ts;
            UInt32 frames = m_now_capture_frames - m_prev_capture_frames;
            if ((time > 0) && ((time > 2000) || (frames > 100))) {
                m_prev_ts = now_ts;
                m_prev_capture_frames = m_now_capture_frames;
                m_video_capture_fps = (Double)frames*1000.0 / time;
            }
            return m_video_capture_fps;
        }
    }
}