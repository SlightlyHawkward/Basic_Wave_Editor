
// WaveEditView.cpp : implementation of the CWaveEditView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WaveEdit.h"
#endif

#include "WaveEditDoc.h"
#include "WaveEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWaveEditView

IMPLEMENT_DYNCREATE(CWaveEditView, CScrollView)

BEGIN_MESSAGE_MAP(CWaveEditView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_CUT, &CWaveEditView::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CWaveEditView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CWaveEditView::OnEditPaste)
	ON_COMMAND(ID_TOOLS_SPEEDUP, &CWaveEditView::OnToolsSpeedup)
	ON_COMMAND(ID_TOOLS_SLOWDOWN, &CWaveEditView::OnToolsSlowdown)
	ON_COMMAND(ID_TOOLS_ECHO, &CWaveEditView::OnToolsEcho)
	ON_COMMAND(ID_VIEW_ZOOM, &CWaveEditView::OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM32776, &CWaveEditView::OnViewZoomOut)
	ON_COMMAND(ID_TOOLS_REVERSE, &CWaveEditView::OnToolsReverse)
END_MESSAGE_MAP()

// CWaveEditView construction/destruction

CWaveEditView::CWaveEditView()
{
	mousePressed = false;
	selectionStart = 0;
	selectionEnd = 0;
	scaleFactor = 500;

}

CWaveEditView::~CWaveEditView()
{
}

BOOL CWaveEditView::PreCreateWindow(CREATESTRUCT& cs)
{

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CWaveEditView drawing

void CWaveEditView::OnDraw(CDC* pDC)
{
	//Get dimensions of the window
	CRect rect;
	GetClientRect(rect);

	//Draw the wave
	CWaveEditDoc* pDoc = GetDocument(); 

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	WaveFile * wave = &pDoc->wave;

	if(wave->hdr == NULL){
		return;
	}

	//Set color in pen and brush for selection
	COLORREF color = RGB(255, 200, 200);
	CPen pen1(PS_SOLID, 0, color);
	pDC->SelectObject(&pen1);
	CBrush brush1(color);
	pDC->SelectObject(&brush1);

	//Draw the selection, if it exists
	if(selectionStart != selectionEnd){
		pDC->Rectangle(selectionStart, 0, selectionEnd, rect.Height());
	}



	//Set color for both pen and brush for wave
	color = RGB(0, 255, 0);
	CPen pen2(PS_SOLID, 0, color);
	pDC->SelectObject(&pen2);
	CBrush brush2(color);
	pDC->SelectObject(&brush2);

	//Draw the wave
	pDC->MoveTo(0, 0);
//	for(int x = 0; x < rect.Width(); x++){

	int width = (wave->maxSamples / wave->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

	for(int x = 0; x < width; x++){
	/*Assuming the whole file will be fit in the window, for every x val in the window
		* we need to find the equivalent sameple in the wave file.*/
		
		float val = wave->get_sample((int)(x * wave->lastSample / width));
		
		/*We need to fit the sound also in the y axis. The y axis goes from 0 in the 
		* top of the window to rect.Height at the bottom. The sound goes from
		*-32768 to 32767, so we scale it in that manner*/ 

		int y = (int)((val + 32768) * (rect.Height() - 1) / (32767 + 32768));
		pDC->LineTo(x, rect.Height() - y);
	}
	
	CSize sizeTotal;
	sizeTotal.cx = width; //Canvas size
	sizeTotal.cy = 0;
	SetScrollSizes(MM_TEXT, sizeTotal);

}


// CWaveEditView printing

BOOL CWaveEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWaveEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWaveEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWaveEditView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	//Intiail Scroll sizes, will probably change later
	CSize sizeTotal;
	sizeTotal.cx = 10000;
	sizeTotal.cy = 0;
	SetScrollSizes(MM_TEXT, sizeTotal);

}

// CWaveEditView diagnostics

#ifdef _DEBUG
void CWaveEditView::AssertValid() const
{
	CView::AssertValid();
}

void CWaveEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWaveEditDoc* CWaveEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWaveEditDoc)));
	return (CWaveEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CWaveEditView message handlers

/*Pressing of the left button handler*/
void CWaveEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	mousePressed = true;

	selectionStart = point.x + GetScrollPosition().x;
	selectionEnd = point.x + GetScrollPosition().x;
	CScrollView::OnLButtonDown(nFlags, point);
}

/*Release of the left button handler*/
void CWaveEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	mousePressed = false;
	this->selectionEnd = point.x + GetScrollPosition().x;
	/*If someone is selecting backwards*/

	if(this->selectionEnd < this->selectionStart){
		int temp = this->selectionStart;
		this->selectionStart = this->selectionEnd;
		this->selectionEnd = temp;
	}

	this->RedrawWindow();
	CScrollView::OnLButtonUp(nFlags, point);
}


/*Mouse movement handler*/
void CWaveEditView::OnMouseMove(UINT nFlags, CPoint point)
{
	
	CScrollView::OnMouseMove(nFlags, point);

	if(mousePressed){
		selectionEnd = point.x + GetScrollPosition().x;
		RedrawWindow();
	}

}


void CWaveEditView::OnEditCut()
{
	CWaveEditDoc* pDoc = GetDocument();
    
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * wave = &pDoc->wave;
	CWaveEditApp* app = dynamic_cast<CWaveEditApp*>(AfxGetApp());

	app->clipboard = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample); //Needs to be cleansed after its used once

    if (wave->hdr==NULL) {
        return;
    }

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	int width = (wave->maxSamples / wave->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

    // Scale the start and end of the selection.
	double startms = (1000.0 * wave->lastSample /wave->sampleRate) * this->selectionStart/width;

    // Scale the start and end of the selection.
    double endms = (1000.0 * wave->lastSample /wave->sampleRate) * this->selectionEnd/width;
    
    // Copy first the fragment to the clipboard
    app->clipboard = wave->get_fragment(startms, endms);

    // Remove the fragment
    WaveFile * w2 = wave->remove_fragment(startms, endms);

    // Substitute old wave with new one
    pDoc->wave = *w2;

    // Remove old wave
    //delete wave;

	this->selectionStart = 0;
	this->selectionEnd = 0;

	wave->updateHeader();

    // Update window
    this->RedrawWindow();

}


void CWaveEditView::OnEditCopy()
{
	CWaveEditDoc* pDoc = GetDocument();
	CWaveEditApp* app = dynamic_cast<CWaveEditApp*>(AfxGetApp());
    
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * wave = &pDoc->wave;
	app->clipboard = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);

    if (wave->hdr==NULL) {
        return;
    }

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	int width = (wave->maxSamples / wave->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

    // Scale the start and end of the selection.
    double startms = (1000.0 * wave->lastSample /wave->sampleRate) * this->selectionStart/width;

    // Scale the start and end of the selection.
    double endms = (1000.0 * wave->lastSample /wave->sampleRate) * this->selectionEnd/width;
    
    // Copy first the fragment to the clipboard
    app->clipboard = wave->get_fragment(startms, endms);

	app->clipboard->updateHeader();

	this->selectionStart = 0;
	this->selectionEnd = 0;

}


void CWaveEditView::OnEditPaste()
{
	CWaveEditDoc* pDoc = GetDocument();
	CWaveEditApp* app = dynamic_cast<CWaveEditApp*>(AfxGetApp());

    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * wave = &pDoc->wave;
	
    if (wave->hdr==NULL) {
        return;
	} if(app->clipboard == NULL){
		return;
	}

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	int width = (wave->maxSamples / wave->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

    // Scale the start and end of the selection.
    double startms = (1000.0 * wave->lastSample /wave->sampleRate) * this->selectionStart/width;

	WaveFile * w2 = wave->paste_fragment(startms, app->clipboard);

	pDoc->wave = *w2;
	wave->updateHeader();

	this->RedrawWindow();
}


void CWaveEditView::OnToolsSpeedup()
{
	CWaveEditDoc* pDoc = GetDocument();
    
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * w2 = &pDoc->wave;
	
    if (w2->hdr==NULL) {
        return;
    }

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	int width = (w2->maxSamples / w2->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

	double startms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionStart/width;
	double endms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionEnd/width;
 
	float speed = 2;

	if(startms == endms){
		w2 = w2->changeSpeed(speed);
		pDoc->wave = *w2;
	} else {   
		WaveFile * edit = new WaveFile(w2->numChannels, w2->sampleRate, w2->bitsPerSample);
		edit = w2->get_fragment(startms, endms);
		edit = edit->changeSpeed(speed);
		
		w2 = w2->replace_fragment(startms, endms, edit);
		w2->updateHeader();
		pDoc->wave = *w2;
	}

	this->selectionStart = 0;
	this->selectionEnd = 0;
	
	this->RedrawWindow();
}


void CWaveEditView::OnToolsSlowdown()
{
	CWaveEditDoc* pDoc = GetDocument();
    
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * w2 = &pDoc->wave;
	
    if (w2->hdr==NULL) {
        return;
    }

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	float speed = .5;

	int width = (w2->maxSamples / w2->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

	double startms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionStart/width;
	double endms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionEnd/width;
 
	if(startms == endms){
		w2 = w2->changeSpeed(speed);
		pDoc->wave = *w2;
	} else {   
		WaveFile * edit = new WaveFile(w2->numChannels, w2->sampleRate, w2->bitsPerSample);
		edit = w2->get_fragment(startms, endms);
		edit = edit->changeSpeed(speed);		
		w2 = w2->replace_fragment(startms, endms, edit);
		w2->updateHeader();
		pDoc->wave = *w2;
	}

	this->selectionStart = 0;
	this->selectionEnd = 0;

	this->RedrawWindow();
}


void CWaveEditView::OnToolsEcho()
{
	CWaveEditDoc* pDoc = GetDocument();
    
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * w2 = &pDoc->wave;
	
    if (w2->hdr==NULL) {
        return;
    }

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	float attenuation = .2;
	float delay = 100; // In MS

	int width = (w2->maxSamples / w2->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

	double startms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionStart/width;
	double endms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionEnd/width;
 
	if(startms == endms){
		w2 = w2->echo(attenuation, delay);
		w2->updateHeader();
		pDoc->wave = *w2;
	} else {   
		WaveFile * edit = new WaveFile(w2->numChannels, w2->sampleRate, w2->bitsPerSample);
		edit = w2->get_fragment(startms, endms);
		edit->updateHeader();
		edit = edit->echo(attenuation, delay);
		edit->updateHeader();
		
		w2 = w2->replace_fragment(startms, endms, edit);
		w2->updateHeader();
		pDoc->wave = *w2;
	}

	this->selectionStart = 0;
	this->selectionEnd = 0;

	this->RedrawWindow();
	
}


void CWaveEditView::OnViewZoomIn()
{
	scaleFactor += scaleFactor/2;
	this->RedrawWindow();

}


void CWaveEditView::OnViewZoomOut()
{
	scaleFactor -= scaleFactor/2;
	this->RedrawWindow();

}


void CWaveEditView::OnToolsReverse()
{

	CWaveEditDoc* pDoc = GetDocument();
    
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    WaveFile * w2 = &pDoc->wave;
	
    if (w2->hdr==NULL) {
        return;
    }

    // Get dimensions of the window.
    CRect rect;
    GetClientRect(rect);

	int width = (w2->maxSamples / w2->sampleRate) * scaleFactor ; //Samplerate * (pixSecond/framerate)

	double startms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionStart/width;
	double endms = (1000.0 * w2->lastSample /w2->sampleRate) * this->selectionEnd/width;
 
	if(startms == endms){
		w2 = w2->reverse();
		w2->updateHeader();
		pDoc->wave = *w2;
	} else {   
		WaveFile * edit = new WaveFile(w2->numChannels, w2->sampleRate, w2->bitsPerSample);
		edit = w2->get_fragment(startms, endms);
		edit->updateHeader();
		edit = edit->reverse();
		edit->updateHeader();
		
		w2 = w2->replace_fragment(startms, endms, edit);
		w2->updateHeader();
		pDoc->wave = *w2;
	}

	this->selectionStart = 0;
	this->selectionEnd = 0;

	this->RedrawWindow();
	

}

