#pragma once

#include <opencv2/opencv.hpp>

class CPCBAnalyzerView : public CView
{
protected: // create from serialization only
    CPCBAnalyzerView() noexcept;
    DECLARE_DYNCREATE(CPCBAnalyzerView)

public:
  // CPCBAnalyzerDoc* GetDocument() const;

    // Attributes
private:
    cv::Mat m_image; // Stores the uploaded image

    // Operations
public:
    void OnFileUploadImage();
    void OnFileDetectCircles();
    afx_msg void OnFileAddTangents();
    //std::vector<cv::Vec3f> m_detectedCircles;
  
    // Overrides
public:
    virtual void OnDraw(CDC* pDC);

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    //virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    //virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

    // Implementation
public:
    virtual ~CPCBAnalyzerView();

#ifdef _DEBUG
   /* virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;*/
#endif

protected:
   /* afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);*/

    DECLARE_MESSAGE_MAP()
};
