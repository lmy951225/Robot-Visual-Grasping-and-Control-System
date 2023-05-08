#include "../common/common.hpp"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

double T1[4][4]{-0.779784,-0.669969,-19.6291,44.4112,0.746169,0.597669,3.37789,-7.06904,-0.362469,0.198704,-7.56281,17.0382,0,0,0,1};//转换方程
//double P1[4][1]{0,0,0,1};//用于存放相机捕获的像素坐标点
//double P2[4][1]{0,0,0,1};//用于存放像素点转换为相机坐标点
//double R1[4][1]{};//同与存放相机坐标点计算得到的机器人坐标点

void cameraCal(double M[4][1] ,double P[4][1])
{
     
    P[0][0] = M[2][0] * (M[0][0] - 622.098999) / 1080.458740 / 1000;
    P[1][0] = M[2][0] * (M[1][0] - 460.747192) / 1080.458740 / 1000;
    P[2][0] = M[2][0] / 1000;
    
}

void robotCal(double T[4][4],double P[4][1],double R[4][1])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                R[i][j] += T[i][k] * P[k][j];
            }
            
        }
        
    }
    
    
}
void on_mouse(int event,int x,int y,int flags,void*)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        printf("当前像素点位置为:");
        std::cout << cv::Point(x,y) << std::endl;
    }
    
}

void eventCallback(TY_EVENT_INFO *event_info, void *userdata)
{
    if (event_info->eventId == TY_EVENT_DEVICE_OFFLINE) {
        LOGD("=== Event Callback: Device Offline!");
        // Note: 
        //     Please set TY_BOOL_KEEP_ALIVE_ONOFF feature to false if you need to debug with breakpoint!
    }
    else if (event_info->eventId == TY_EVENT_LICENSE_ERROR) {
        LOGD("=== Event Callback: License Error!");
    }
}

int main(int argc, char* argv[])
{
    std::string ID, IP;
    TY_INTERFACE_HANDLE hIface = NULL;
    TY_ISP_HANDLE hColorIspHandle = NULL;
    TY_DEV_HANDLE hDevice = NULL;
    int32_t color, ir, depth;
    color = ir = depth = 1;

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-id") == 0){
            ID = argv[++i];
        } else if(strcmp(argv[i], "-ip") == 0) {
            IP = argv[++i];
        } else if(strcmp(argv[i], "-color=off") == 0) {
            color = 0;
        } else if(strcmp(argv[i], "-depth=off") == 0) {
            depth = 0;
        } else if(strcmp(argv[i], "-ir=off") == 0) {
            ir = 0;
        } else if(strcmp(argv[i], "-h") == 0) {
            LOGI("Usage: SimpleView_FetchFrame [-h] [-id <ID>] [-ip <IP>]");
            return 0;
        }
    }

    if (!color && !depth && !ir) {
        LOGD("At least one component need to be on");
        return -1;
    }

    LOGD("Init lib");
    ASSERT_OK( TYInitLib() );
    TY_VERSION_INFO ver;
    ASSERT_OK( TYLibVersion(&ver) );
    LOGD("     - lib version: %d.%d.%d", ver.major, ver.minor, ver.patch);

    std::vector<TY_DEVICE_BASE_INFO> selected;
    ASSERT_OK( selectDevice(TY_INTERFACE_ALL, ID, IP, 1, selected) );
    ASSERT(selected.size() > 0);
    TY_DEVICE_BASE_INFO& selectedDev = selected[0];

    ASSERT_OK( TYOpenInterface(selectedDev.iface.id, &hIface) );
    ASSERT_OK( TYOpenDevice(hIface, selectedDev.id, &hDevice) );

    int32_t allComps;
    ASSERT_OK( TYGetComponentIDs(hDevice, &allComps) );

    ///try to enable color camera
    if(allComps & TY_COMPONENT_RGB_CAM  && color) {
        LOGD("Has RGB camera, open RGB cam");
        ASSERT_OK( TYEnableComponents(hDevice, TY_COMPONENT_RGB_CAM) );
        //create a isp handle to convert raw image(color bayer format) to rgb image
        ASSERT_OK(TYISPCreate(&hColorIspHandle));
        //Init code can be modified in common.hpp
        //NOTE: Should set RGB image format & size before init ISP
        ASSERT_OK(ColorIspInitSetting(hColorIspHandle, hDevice));
        //You can  call follow function to show  color isp supported features
#if 0
        ColorIspShowSupportedFeatures(hColorIspHandle);
#endif
        //You can turn on auto exposure function as follow ,but frame rate may reduce .
        //Device may be casually stucked  1~2 seconds while software is trying to adjust device exposure time value
#if 0
        ASSERT_OK(ColorIspInitAutoExposure(hColorIspHandle, hDevice));
#endif
    }

    if (allComps & TY_COMPONENT_IR_CAM_LEFT && ir) {
        LOGD("Has IR left camera, open IR left cam");
        ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_IR_CAM_LEFT));
    }

    if (allComps & TY_COMPONENT_IR_CAM_RIGHT && ir) {
        LOGD("Has IR right camera, open IR right cam");
        ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_IR_CAM_RIGHT));
    }

    //try to enable depth map
    LOGD("Configure components, open depth cam");
    DepthViewer depthViewer("Depth");
    if (allComps & TY_COMPONENT_DEPTH_CAM && depth) {
        //int32_t image_mode;
        //ASSERT_OK(get_default_image_mode(hDevice, TY_COMPONENT_DEPTH_CAM, image_mode));
        //LOGD("Select Depth Image Mode: %dx%d", TYImageWidth(image_mode), TYImageHeight(image_mode));
        LOGD("=== Configure feature, set resolution to 1280x960.");
        ASSERT_OK(TYSetEnum(hDevice, TY_COMPONENT_DEPTH_CAM, TY_ENUM_IMAGE_MODE, TY_IMAGE_MODE_DEPTH16_1280x960));
        ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_DEPTH_CAM));

        //depth map pixel format is uint16_t ,which default unit is  1 mm
        //the acutal depth (mm)= PixelValue * ScaleUnit 
        float scale_unit = 1.;
        TYGetFloat(hDevice, TY_COMPONENT_DEPTH_CAM, TY_FLOAT_SCALE_UNIT, &scale_unit);
        depthViewer.depth_scale_unit = scale_unit;
    }



    LOGD("Prepare image buffer");
    uint32_t frameSize;
    ASSERT_OK( TYGetFrameBufferSize(hDevice, &frameSize) );
    LOGD("     - Get size of framebuffer, %d", frameSize);

    LOGD("     - Allocate & enqueue buffers");
    char* frameBuffer[2];
    frameBuffer[0] = new char[frameSize];
    frameBuffer[1] = new char[frameSize];
    LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[0], frameSize);
    ASSERT_OK( TYEnqueueBuffer(hDevice, frameBuffer[0], frameSize) );
    LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[1], frameSize);
    ASSERT_OK( TYEnqueueBuffer(hDevice, frameBuffer[1], frameSize) );

    LOGD("Register event callback");
    ASSERT_OK(TYRegisterEventCallback(hDevice, eventCallback, NULL));

    bool hasTrigger;
    ASSERT_OK(TYHasFeature(hDevice, TY_COMPONENT_DEVICE, TY_STRUCT_TRIGGER_PARAM, &hasTrigger));
    if (hasTrigger) {
        LOGD("Disable trigger mode");
        TY_TRIGGER_PARAM trigger;
        trigger.mode = TY_TRIGGER_MODE_OFF;
        ASSERT_OK(TYSetStruct(hDevice, TY_COMPONENT_DEVICE, TY_STRUCT_TRIGGER_PARAM, &trigger, sizeof(trigger)));
    }

    LOGD("Start capture");
    ASSERT_OK( TYStartCapture(hDevice) );

    LOGD("While loop to fetch frame");
    //创建管道，实现相机与机器人的通信
    int ret = access("fifo2",F_OK);
	if (ret == -1)
	{
		printf("管道文件不存在，创建对应的有名管道\n");
		ret = mkfifo("fifo2",0664);
		if (ret == -1)
		{
			perror("mkfifo");
			exit(0);
		}
		
	}

	// 以只写的方式打开管道fifo2
	int fdw = open("fifo2",O_WRONLY);
	if (fdw == -1)
	{
		perror("open");
		exit(0);
	}
	printf("打开管道fifo2成功,等待相机发送指令...\n");
	char buf1[128];

    bool exit_main = false;
    TY_FRAME_DATA frame;
    int index = 0;
    while(!exit_main) {
        int err = TYFetchFrame(hDevice, &frame, -1);
        if( err == TY_STATUS_OK ) {
            //LOGD("Get frame %d", ++index);

            int fps = get_fps();
            // if (fps > 0){
            //     LOGI("fps: %d", fps);
            // }

            cv::Mat depth, irl, irr, color;
            parseFrame(frame, &depth, &irl, &irr, &color, hColorIspHandle);
            if(!depth.empty()){
                depthViewer.show(depth);
                
            }

            
            //if(!irl.empty()){ cv::imshow("LeftIR", irl); }
            //if(!irr.empty()){ cv::imshow("RightIR", irr); }
            if(!color.empty())
            {   
                
                cv::imshow("Color", color); 
                cv::setMouseCallback("Color",on_mouse,0);
            }
            
            int key = cv::waitKey(1);
            switch(key & 0xff) {
            case 0xff:
                break;
            case 'q':
                exit_main = true;
                break;
            case 's':
                {
                    if (!(depthViewer.x == 0 && depthViewer.y == 0 && depthViewer.deepth == 0))
                    {
                    
                        double P1[4][1]{0,0,0,1};
                        P1[0][0] = depthViewer.x;
                        P1[1][0] = depthViewer.y;
                        P1[2][0] = depthViewer.deepth;
                        printf("选取点的像素坐标为:\n");
                        for (int i = 0; i < 4; i++)
                        {
                            for (int k = 0; k < 1; k++)
                            {
                                std::cout << P1[i][k] << " ";
                            }
                            std::cout << std::endl;
                        }
                        double P2[4][1]{0,0,0,1};
                        cameraCal(P1,P2);
                        printf("相机坐标系下坐标为:\n");
                        for (int i = 0; i < 4; i++)
                        {
                            for (int k = 0; k < 1; k++)
                            {
                                std::cout << P2[i][k] << " ";
                            }
                            std::cout << std::endl;
                        }
                        double R1[4][1]{};
                        robotCal(T1,P2,R1);
                        printf("要发送的机器人坐标点为:\n");
                        for (int i = 0; i < 4; i++)
                        {
                            for (int k = 0; k < 1; k++)
                            {
                                std::cout << R1[i][k] << " ";
                            }
                            std::cout << std::endl;
                        }
                        memset(buf1,0,128);
                        sprintf(buf1, "[%f,%f,%f]", R1[0][0], R1[1][0], R1[2][0]);
                        
                        printf("发送指令...\n");
                        //写数据
                        ret = write(fdw,buf1,strlen(buf1));
                        if (ret == -1)
                        {
                            perror("write");
                            exit(0);
                        }
                        
                    }
                    break;
                }
            default:
                LOGD("Unmapped key %d", key);
            }

            TYISPUpdateDevice(hColorIspHandle);
            //LOGD("Re-enqueue buffer(%p, %d)"
            //    , frame.userBuffer, frame.bufferSize);
            ASSERT_OK( TYEnqueueBuffer(hDevice, frame.userBuffer, frame.bufferSize) );
        }
    }
    ASSERT_OK( TYStopCapture(hDevice) );
    ASSERT_OK( TYCloseDevice(hDevice));
    ASSERT_OK( TYCloseInterface(hIface) );
    ASSERT_OK(TYISPRelease(&hColorIspHandle));
    ASSERT_OK( TYDeinitLib() );
    delete frameBuffer[0];
    delete frameBuffer[1];
    close(fdw);
    LOGD("Main done!");
    return 0;
}
