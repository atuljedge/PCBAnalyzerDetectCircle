#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "PCBAnalyzer.h"
#endif

#include "PCBAnalyzerDoc.h"
#include "PCBAnalyzerView.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>

using namespace cv;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<cv::Vec3f> circles;

IMPLEMENT_DYNCREATE(CPCBAnalyzerView, CView)

BEGIN_MESSAGE_MAP(CPCBAnalyzerView, CView)
    ON_COMMAND(ID_FILE_UPLOAD_IMAGE, &CPCBAnalyzerView::OnFileUploadImage)
    ON_COMMAND(ID_FILE_DETECT_CIRCLES, &CPCBAnalyzerView::OnFileDetectCircles)
    ON_COMMAND(ID_FILE_ADD_TANGENTS, &CPCBAnalyzerView::OnFileAddTangents)
END_MESSAGE_MAP()

CPCBAnalyzerView::CPCBAnalyzerView() noexcept
{
}

CPCBAnalyzerView::~CPCBAnalyzerView()
{
}

BOOL CPCBAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CPCBAnalyzerView::OnDraw(CDC* pDC)
{
    if (m_image.empty())
        return;

    CRect rect;
    GetClientRect(&rect);

    cv::Mat resizedImage;
    float imgAspect = (float)m_image.cols / (float)m_image.rows;
    float wndAspect = (float)rect.Width() / (float)rect.Height();

    if (imgAspect > wndAspect)
    {
        int newHeight = static_cast<int>(rect.Width() / imgAspect);
        cv::resize(m_image, resizedImage, cv::Size(rect.Width(), newHeight));
    }
    else
    {
        int newWidth = static_cast<int>(rect.Height() * imgAspect);
        cv::resize(m_image, resizedImage, cv::Size(newWidth, rect.Height()));
    }

    CImage cimage;
    cimage.Create(resizedImage.cols, resizedImage.rows, 24);

    for (int y = 0; y < resizedImage.rows; y++)
    {
        uchar* srcRow = resizedImage.ptr(y);
        uchar* destRow = (uchar*)cimage.GetPixelAddress(0, y);
        memcpy(destRow, srcRow, resizedImage.cols * 3);
    }

    cimage.Draw(pDC->m_hDC, 0, 0);
}

void CPCBAnalyzerView::OnFileUploadImage()
{
    CFileDialog dlg(TRUE, _T("bmp"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        _T("Image Files (*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*||"));

    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();
        std::string filePathStd = std::string(CT2A(filePath)); // Convert CString to std::string

        m_image = cv::imread(filePathStd, cv::IMREAD_COLOR);
        if (m_image.empty())
        {
            AfxMessageBox(_T("Failed to load the image."));
            return;
        }

        Invalidate(); // Request redraw of the view
    }
}

void CPCBAnalyzerView::OnFileDetectCircles()
{
    if (m_image.empty())
    {
        AfxMessageBox(_T("Please upload an image first."));
        return;
    }

    cv::Mat grayImage;
    cv::cvtColor(m_image, grayImage, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(grayImage, grayImage, cv::Size(9, 9), 2, 2);

 
    cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows / 8, 100, 30, 10, 10000);

    if (circles.empty())
    {
        AfxMessageBox(_T("No circles were detected."));
        return;
    }

    for (const auto& circle : circles)
    {
        // Extract circle details
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);

        // Draw the circle and its center
        cv::circle(m_image, center, radius, cv::Scalar(0, 255, 0), 2); // Circle outline
        cv::circle(m_image, center, 2, cv::Scalar(0, 0, 255), 3);      // Circle center

        // Display center coordinates
        CString centerCoordsStr;
        centerCoordsStr.Format(_T("(%d, %d)"), center.x, center.y);
        std::string centerCoordsStd = std::string(CT2A(centerCoordsStr));
        cv::putText(m_image, centerCoordsStd, center + cv::Point(5, 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    for (size_t i = 0; i < circles.size(); ++i)
    {
        for (size_t j = i + 1; j < circles.size(); ++j)
        {
            // Center points and radii
            cv::Point2f center1(circles[i][0], circles[i][1]);
            cv::Point2f center2(circles[j][0], circles[j][1]);

            // Draw line between centers
            cv::line(m_image, center1, center2, cv::Scalar(255, 0, 0), 1);

            // Calculate distance between centers
            float distance = cv::norm(center1 - center2);

            // Display distance at the midpoint
            cv::Point midpoint((center1.x + center2.x) / 2, (center1.y + center2.y) / 2);
            CString distanceStr;
            distanceStr.Format(_T("Dist: %.2f"), distance);
            std::string distanceStrStd = std::string(CT2A(distanceStr));
            cv::putText(m_image, distanceStrStd, midpoint, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
        }
    }

    Invalidate(); // Redraw to show updated image with calculations
}


void CPCBAnalyzerView::OnFileAddTangents()
{
    if (m_image.empty())
    {
        AfxMessageBox(_T("Please upload an image first."));
        return;
    }
    
    //// Convert the image to grayscale
    //cv::Mat grayImage;
    //cv::cvtColor(m_image, grayImage, cv::COLOR_BGR2GRAY);
    //cv::GaussianBlur(grayImage, grayImage, cv::Size(9, 9), 2, 2);

    //// Detect circles using HoughCircles
    //std::vector<cv::Vec3f> circles;
    //cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows / 8, 100, 30, 10, 10000);

    if (circles.size() < 2)
    {
        AfxMessageBox(_T("At least two circles must be detected to add tangents."));
        return;
    }

    // Iterate through each pair of circles
    for (size_t i = 0; i < circles.size(); ++i)
    {
        for (size_t j = i + 1; j < circles.size(); ++j)
        {
            // Circle 1: Center and Radius
            cv::Point2f center1(circles[i][0], circles[i][1]);
            float radius1 = circles[i][2];

            // Circle 2: Center and Radius
            cv::Point2f center2(circles[j][0], circles[j][1]);
            float radius2 = circles[j][2];

            // Calculate distance between the centers
            float d = cv::norm(center1 - center2);

            // Skip if the circles overlap
            if (d <= std::fabs(radius1 - radius2))
                continue;

            // Calculate the angle between centers
            float angle = std::atan2(center2.y - center1.y, center2.x - center1.x);   // calculates angle between centre distance line &  x axis 

            // Direct Common Tangents (Green Lines)
            float alpha = std::acos((radius1 - radius2) / d);

            cv::Point2f p1a(
                center1.x + radius1 * std::cos(angle + alpha),
                center1.y + radius1 * std::sin(angle + alpha)
            );
            cv::Point2f p2a(
                center2.x + radius2 * std::cos(angle + alpha),
                center2.y + radius2 * std::sin(angle + alpha)
            );

            cv::Point2f p1b(
                center1.x + radius1 * std::cos(angle - alpha),
                center1.y + radius1 * std::sin(angle - alpha)
            );
            cv::Point2f p2b(
                center2.x + radius2 * std::cos(angle - alpha),
                center2.y + radius2 * std::sin(angle - alpha)
            );

            cv::line(m_image, p1a, p2a, cv::Scalar(0, 255, 0), 2); // Green line
            cv::line(m_image, p1b, p2b, cv::Scalar(0, 255, 0), 2); // Green line

            // Transverse Common Tangents (Blue Lines)
            if (d > radius1 + radius2)
            {
                float beta = std::acos((radius1 + radius2) / d);

                cv::Point2f p1c(
                    center1.x + radius1 * std::cos(angle + beta),
                    center1.y + radius1 * std::sin(angle + beta)
                );
                cv::Point2f p2c(
                    center2.x - radius2 * std::cos(angle + beta),
                    center2.y - radius2 * std::sin(angle + beta)
                );

                cv::Point2f p1d(
                    center1.x + radius1 * std::cos(angle - beta),
                    center1.y + radius1 * std::sin(angle - beta)
                );
                cv::Point2f p2d(
                    center2.x - radius2 * std::cos(angle - beta),
                    center2.y - radius2 * std::sin(angle - beta)
                );

                cv::line(m_image, p1c, p2c, cv::Scalar(255, 0, 0), 2); // Blue line
                cv::line(m_image, p1d, p2d, cv::Scalar(255, 0, 0), 2); // Blue line
            }
        }
    }

    Invalidate(); // Redraw to show updated image with tangents
}





