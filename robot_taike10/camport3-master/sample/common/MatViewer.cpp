/*
 * @Author: lmy mingyue.li@grandhonor.com
 * @Date: 2023-04-26 09:03:03
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-04-27 13:21:37
 * @FilePath: /amc-1.5/camport3-master/sample/common/MatViewer.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEx
 */
#include <stdint.h>
#include <stdio.h>
#include "MatViewer.hpp"
#include <iostream>

int GraphicItem::globalID = 0;


void OpencvViewer::_onMouseCallback(int event, int x, int y, int /*flags*/, void* ustc)
{
    OpencvViewer* p = (OpencvViewer*)ustc;

    // NOTE: This callback will be called very frequently while mouse moving,
    // keep it simple

    bool repaint = false;
    p->onMouseCallback(p->_orgImg, event, cv::Point(x,y), repaint);
    if(repaint){
        p->showImage();
    }
}


void OpencvViewer::showImage()
{
    _showImg = _orgImg.clone();
    for(std::map<int, GraphicItem*>::iterator it = _items.begin()
            ; it != _items.end(); it++){
        it->second->draw(_showImg);
    }
    cv::imshow(_win.c_str(), _showImg);
    cv::setMouseCallback(_win, _onMouseCallback, this);
}

///////////////////////////// DepthViewer ///////////////////////////////////////


DepthViewer::DepthViewer(const std::string& win)
    : OpencvViewer(win)
    , _centerDepthItem(std::string(), cv::Point(0,20), 0.5, cv::Scalar(0,255,0), 2)
    , _pickedDepthItem(std::string(), cv::Point(0,40), 0.5, cv::Scalar(0,255,0), 2)
{
    OpencvViewer::addGraphicItem(&_centerDepthItem);
    OpencvViewer::addGraphicItem(&_pickedDepthItem);
    depth_scale_unit = 1.f;
}


void DepthViewer::show(const cv::Mat& img)
{
    if(img.type() != CV_16U || img.total() == 0){
        return;
    }

    char str[128];
    float val = img.at<uint16_t>(img.rows / 2, img.cols / 2)*depth_scale_unit;
    sprintf(str, "Depth at center: %.1f", val);
    _centerDepthItem.set(str);

    val = img.at<uint16_t>(_fixLoc.y, _fixLoc.x)*depth_scale_unit;
    sprintf(str, "Depth at (%d,%d): %.1f", _fixLoc.x, _fixLoc.y , val);
    _pickedDepthItem.set(str);

    _depth = img.clone();
    _renderedDepth = _render.Compute(img);
    OpencvViewer::show(_renderedDepth);
}


void DepthViewer::onMouseCallback(cv::Mat& img, int event, const cv::Point pnt
    , bool& repaint)
{
    repaint = false;
    switch(event){
        case cv::EVENT_LBUTTONDOWN: {
            _fixLoc = pnt;
            char str[128];
            float val = _depth.at<uint16_t>(pnt.y, pnt.x)*depth_scale_unit;
            sprintf(str, "Depth at (%d,%d): %.1f", pnt.x, pnt.y, val);
            //printf(">>>>>>>>>>>>>>>> depth(%.1f)\n", val);
            _pickedDepthItem.set(str);
            double T1[4][1] = {pnt.x,pnt.y,val,1};
            printf("选取点的像素坐标为:\n");
            for (int i = 0; i < 4; i++)
            {
                for (int k = 0; k < 1; k++)
                {
                    std::cout << T1[i][k] << " ";
                }
                
                std::cout << std::endl;
            }
            x = pnt.x;
            y = pnt.y;
            deepth = val;
            repaint = true;
            break;
        }
        case cv::EVENT_MOUSEMOVE: 
            // uint16_t val = _img.at<uint16_t>(pnt.y, pnt.x);
            // char str[32];
            // sprintf(str, "Depth at mouse: %d", val);
            // drawText(img, str, cv::Point(0,60), 0.5, cv::Scalar(0,255,0), 2);
            _fixLoc = pnt;
            char str[128];
            float val = _depth.at<uint16_t>(pnt.y, pnt.x)*depth_scale_unit;
            sprintf(str, "Depth at (%d,%d): %.1f", pnt.x, pnt.y, val);
            //printf(">>>>>>>>>>>>>>>> depth(%.1f)\n", val);
            _pickedDepthItem.set(str);
            repaint = true;
            break;
    }
}

