/****************************************************************************

	QXDirect Class
	Copyright (C) 2008-2009 Andre Deperrois adeperrois@xflr5.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

/** @file This file implements the QXDirect class used to manage direct Foil analysis. */

#ifndef QXDIRECT_H
#define QXDIRECT_H

#include <QPixmap>
#include <QSettings>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QStackedWidget>
#include "../misc/RenameDlg.h"
#include "../misc/LineButton.h"
#include "../misc/LineCbBox.h"
#include "../misc/LineDelegate.h"
#include "../misc/FloatEdit.h"
#include "../misc/PolarFilterDlg.h"
#include "../misc/ObjectPropsDlg.h"
#include "../objects/Polar.h"
#include "../objects/Foil.h"
#include "../objects/OpPoint.h"
#include "../graph/QGraph.h"
#include "../graph/GraphDlg.h"
#include "XFoil.h"
#include "EditPlrDlg.h"
#include "XFoilAnalysisDlg.h"
#include "XFoilAdvancedDlg.h"
#include "FoilPolarDlg.h"
#include "BatchDlg.h"
#include "TwoDPanelDlg.h"
#include "InterpolateFoilsDlg.h"
#include "NacaFoilDlg.h"
#include "ManageFoilsDlg.h"

#include "FoilCoordDlg.h"
#include "FoilGeomDlg.h"
#include "TEGapDlg.h"
#include "LEDlg.h"
#include "FlapDlg.h"
#include "CAddDlg.h"
#include "XDirectStyleDlg.h"

#define MAXPOLARGRAPHS 5


/**
* @class QXDirect This class is the general interface for Foil direct Analysis.
*
* This is the handling class for the QXDirect right toolbar.
* It provides the methods to modify the foil geometry, define the Polar analysis, perform the analysis, and post-process the results.
* One of the very first class of this project.
*/
class QXDirect : public QWidget
{
	friend class MainFrame;
	friend class TwoDWidget;
	friend class XFoilAnalysisDlg;
	friend class BatchDlg;
	friend class BatchThreadDlg;
	friend class FoilPolarDlg;
	friend class EditPlrDlg;
	friend class XDirectStyleDlg;
	friend class DisplaySettingsDlg;

    Q_OBJECT

public:
	QXDirect(QWidget *parent = NULL);
	~QXDirect();

public slots:
	void UpdateView();


private slots:
	void OnXFoilAdvanced();

	void OnCouplePolarGraphs();
	void OnAllPolarGraphs();
	void OnSinglePolarGraph();
	void OnAllPolarGraphsSetting();

	void OnCpGraphSettings();
	void OnPolarFilter();
	void OnInputChanged();
	void OnAnalyze();
	void OnAnimate(bool bChecked);
	void OnAnimateSpeed(int val);
	void OnBatchAnalysis();
	void OnMultiThreadedBatchAnalysis();
	void OnCpi();
	void OnCurOppOnly();
	void OnCurveStyle(int index);
	void OnCurveWidth(int index);
	void OnCurveColor();
	void OnDeleteCurFoil();
	void OnDelCurOpp();
	void OnDeleteCurPolar();
	void OnDeleteFoilPolars();
	void OnEditCurPolar();
	void OnExportCurFoil();
	void OnExportCurOpp();
	void OnExportPolarOpps() ;
	void OnExportCurPolar();
	void OnExportAllPolars();
	void OnHideAllOpps();
	void OnHideAllPolars();
	void OnHideFoilPolars();
	void OnImportJavaFoilPolar();
	void OnImportXFoilPolar();
	void OnInitBL();
	void OnOpPoints();
	void OnPolars();
	void OnResetAllPolarGraphsScales();
	void OnResetFoilScale();
	void OnResetXFoil();
	void OnResetCurPolar();
	void OnSavePolars();
	void OnSequence();
	void OnShowFoilPolarsOnly();
	void OnShowFoilPolars();
	void OnShowBL();
	void OnShowCurve();
	void OnShowNeutralLine();
	void OnShowPoints();
	void OnShowPanels();
	void OnShowAllOpps();
	void OnShowAllPolars();
	void OnShowPressure();
	void OnDefinePolar();
	void OnSpec();
	void OnStoreOpp();
	void OnViscous();
	void OnXDirectStyle();
	void OnGraphSettings();

	void OnShowPolarOpps();
	void OnHidePolarOpps();
	void OnDeletePolarOpps();
	void OnShowFoilOpps();
	void OnHideFoilOpps();
	void OnDeleteFoilOpps();
	void OnDerotateFoil();
	void OnNormalizeFoil();
	void OnCadd();
	void OnPanels();
	void OnFoilCoordinates();
	void OnFoilGeom();
	void OnResetGraphLegend();
	void OnSetTEGap();
	void OnSetLERadius();
	void OnSetFlap();
	void OnInterpolateFoils();
	void OnNacaFoils();
	void OnManageFoils();
	void OnRenamePolar();
	void OnAnimateSingle();
	void OnHighlightOpp();

	void OnQGraph();
	void OnCpGraph();
	void OnExportCurXFoilResults();
	void OnCtPlot();
	void OnDbPlot();
	void OnDtPlot();
	void OnRtLPlot();
	void OnRtPlot();
	void OnNPlot();
	void OnCdPlot();
	void OnCfPlot();
	void OnUePlot();
	void OnHPlot();

	void OnOpPointProps();
	void OnPolarProps();
	void OnRenameFoil();


protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event);
	void mouseDoubleClickEvent ( QMouseEvent * event );

	void AddOpData(OpPoint *pNewPoint);
	void SetControls();
	void Connect();
	void CreateOppCurves(OpPoint *pOpp= NULL);
	void CreatePolarCurves();
	void DeleteFoil(bool bAsk);
	void DeleteOpPoint(bool bCurrent);
	void PaintPolarLegend(QPoint place, int bottom,QPainter &painter);
	void FillComboBoxes(bool bEnable = true);
	void FillPolarCurve(Curve *pCurve, Polar *pPolar, int XVar, int YVar);
	void FillOppCurve(OpPoint *pOpp, Graph *pGraph, Curve *pCurve, bool bInviscid=false);
	void InsertOpPoint(OpPoint *pNewPoint);
	void LoadSettings(QSettings *pSettings);

	void PaintBL(QPainter &painter, OpPoint* pOpPoint, double scale);
	void PaintPressure(QPainter &painter, OpPoint* pOpPoint, double scale);
	void PaintOpPoint(QPainter &painter);
	void PaintSingleGraph(QPainter &painter);
	void PaintCoupleGraphs(QPainter &painter);
	void PaintPolarGraphs(QPainter &painter);
	void PaintView(QPainter &painter);
	void ReadParams();
	void SaveSettings(QSettings *pSettings);
	void SetBufferFoil();
	void SetCurveParams();
	void SetFoilScale();
	void SetFoilScale(QRect CltRect);
	void SetHingeMoments(OpPoint *pOpPoint);
	void SetOpPointSequence();
	void SetAnalysisParams();
	void SetGraphTitles(Graph* pGraph, int iX, int iY);
	void SetGraphTitles(Graph* pGraph);
	void SetPolarLegendPos();
	void SetupLayout();	
	void StopAnimate();
	void UpdateCurve();

	void * GetVariable(Polar *pPolar, int iVar);
	OpPoint* GetOpPoint(double Alpha);

	QGraph* GetGraph(QPoint &pt);

	Foil* SetFoil(Foil* pFoil=NULL);
	Foil* SetFoil(QString FoilName);
	Polar *SetPolar(Polar *pPolar=NULL);
	Polar *SetPolar(QString PlrName);
	OpPoint *SetOpp(double Alpha=-123456789.0);

	OpPoint* AddOpPoint(Polar *pPolar = NULL, bool bStoreOpp=false);



private:
	//the dialog boxes are declared as member variables to enable translations... seems to be a Qt bug
	PolarFilterDlg *m_pPolarFilterDlg;
	ObjectPropsDlg *m_pObjectPropsDlg;
	GraphDlg *m_pGraphDlg;
	RenameDlg *m_pRenameDlg;
	EditPlrDlg *m_pEditPlrDlg;
	XFoilAnalysisDlg *m_pXFoilAnalysisDlg;
	XFoilAdvancedDlg *m_pXFoilAdvancedDlg;
	FoilPolarDlg *m_pFPDlg;
	BatchDlg *m_pBDlg;
//	BatchThreadDlg *m_pBatchThreadDlg;
	XDirectStyleDlg *m_pXDirectStyleDlg;
	ManageFoilsDlg *m_pManageFoilsDlg;

	TwoDPanelDlg *m_pTwoDPanelDlg;
	InterpolateFoilsDlg *m_pIFDlg;
	NacaFoilDlg *m_pNacaFoilDlg;
	FoilCoordDlg *m_pFoilCoordDlg;
	FoilGeomDlg *m_pFoilGeomDlg;
	TEGapDlg *m_pTEGapDlg;
	LEDlg *m_pLEDlg;
	FlapDlg *m_pFlapDlg;
	CAddDlg *m_pCAddDlg;


	QStackedWidget *m_pctrlMiddleControls;

	QTimer *m_pAnimateTimer;

	QLabel *m_pctrlUnit1, *m_pctrlUnit2, *m_pctrlUnit3;

	QRadioButton *m_pctrlSpec1;
	QRadioButton *m_pctrlSpec2;
	QRadioButton *m_pctrlSpec3;

	QCheckBox *m_pctrlSequence;
	FloatEdit *m_pctrlAlphaMin  ;
	FloatEdit *m_pctrlAlphaMax ;
	FloatEdit *m_pctrlAlphaDelta;

	QCheckBox *m_pctrlViscous ;
	QCheckBox *m_pctrlInitBL;
	QCheckBox *m_pctrlStoreOpp;
	QPushButton *m_pctrlAnalyze;

	QCheckBox *m_pctrlShowBL, *m_pctrlShowPressure;
	QCheckBox* m_pctrlAnimate;
	QSlider* m_pctrlAnimateSpeed ;

	QLabel *m_pctrlPolarProps;

	QCheckBox *m_pctrlShowCurve;
	QCheckBox *m_pctrlShowPoints;
	LineCbBox *m_pctrlCurveStyle;
	LineCbBox *m_pctrlCurveWidth;
	LineButton *m_pctrlCurveColor;

	LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;

	bool m_bAlpha;             /**< true if performing an analysis based on aoa, false if based on Cl */
	bool m_bStoreOpp;          /**< true if operating points should be stored after an analysis */
	bool m_bViscous;           /**< true if performing a viscous calculation, false if inviscid */
	bool m_bInitBL;            /**< true if the boundary layer should be initialized for the next xfoil calculation */
	bool m_bBL;                /**< true if the Boundary layer shoud be displayed */
	bool m_bPressure;          /**< true if the pressure distirbution should be displayed */
	bool m_bPolarView;         /**< true if the polar view is selected, false if the operating point view is selected */
	bool m_bShowUserGraph;     /**< true if the 5th polar graph should be displayed */
	bool m_bAnimate;           /**< true if a result animation is underway */
	bool m_bAnimatePlus;       /**< true if the animation is going from lower to higher alpha, false if decreasing */
	bool m_bShowPanels;        /**< true if the panels should be displayed on the foil surface */
	bool m_bType1;             /**< true if the type 1 polars are to be displayed in the graphs */
	bool m_bType2;             /**< true if the type 2 polars are to be displayed in the graphs */
	bool m_bType3;             /**< true if the type 3 polars are to be displayed in the graphs */
	bool m_bType4;             /**< true if the type 4 polars are to be displayed in the graphs */
	bool m_bAutoInitBL;        /**< true if the BL initialization is left to the code's decision */
	bool m_bTrans;             /**< true if the user is dragging a view */
	bool m_bTransGraph;        /**< true if the user is dragging a graph */
	bool m_bFromList;          /**< true if the batch analysis is based on a list of Re values */
	bool m_bFromZero;          /**< true if the batch analysis should start from Alpha=0 */
	bool m_bShowTextOutput;    /**< true if the batch analysis should display text result output */
	bool m_bNeutralLine;       /**< true if the neutral line should be displayed */
	bool m_bCurOppOnly;        /**< true if only the current operating point should be displayed */
	bool m_bShowInviscid;      /**< true if the inviscid results should be displayed */
	bool m_bCpGraph;           /**< true if the Cp graph should be displayed */
	bool m_bSequence;          /**< true if a sequential analysis is to be performed */
	bool m_bXPressed;          /**< true if the 'X' key is pressed */
	bool m_bYPressed;          /**< true if the 'Y' key is pressed */
	bool m_bHighlightOpp;      /**< true if the active operating point should be highlighted on the curves of the polar graphs */

	int m_posAnimate;          /**< the current aoa in the animation */
	int m_XFoilVar;            /**< defines the variable for current XFoil results */
	int m_IterLim;             /**< max iteratio limit for XFoil */

	int m_iPlrGraph;           /**< defines whch polar graph is selected if m_iPlrView=1 */
	enumPolarGraphView m_iPlrView;  /**< defines the number of graphs to be displayed in the polar view */
	int m_FoilYPos;            /**< y position for the foil display, in pixels from the bottom of the screen */

	double m_fFoilScale;        /**< the scale for foil display*/
	double m_ReList[30];        /**< the user-defined list of Re numbers, used for batch analysis */
	double m_MachList[30];      /**< the user-defined list of Mach numbers, used for batch analysis */
	double m_NCritList[30];     /**< the user-defined list of NCrit numbers, used for batch analysis */
	int m_NRe;                  /**< the number of Re values in the ReList */

	double m_Alpha;             /**< the min value of the aoa for a sequential analysis of Type 1, 2, or 3*/
	double m_AlphaMax;          /**< the max value of the aoa for a sequential analysis of Type 1, 2, or 3*/
	double m_AlphaDelta;        /**< the increment value of the aoa for a sequential analysis of Type 1, 2, or 3*/
	double m_Cl;                /**< the min value of the lift coefficient for a sequential analysis of Type 1, 2, or 3*/
	double m_ClMax;             /**< the max value of the aoa for a sequential analysis of Type 1, 2, or 3*/
	double m_ClDelta;           /**< the increment value of the aoa for a sequential analysis of Type 1, 2, or 3*/
	double m_Reynolds;          /**< the min value of the Reynolds number for a sequential analysis of Type 4*/
	double m_ReynoldsMax;       /**< the max value of the Reynolds number for a sequential analysis of Type 4*/
	double m_ReynoldsDelta;     /**< the increment value of the Reynolds number for a sequential analysis of Type 4*/
	double m_Mach;              /**< Stores the Mach number defined in the last Polar creation */
	double m_ASpec;             /**< Stores the aoa defined in the last Type 4 Polar creation */
	double m_NCrit;             /**< Stores the NCrit number defined in the last Polar creation */
	double m_XTopTr;            /**< Stores the position of the top transition point defined in the last Polar creation */
	double m_XBotTr;            /**< Stores the position of the bottom transition point defined in the last Polar creation */
	enumPolarType m_PolarType;  /**< Stores the type of the last polar which has been created. */

	Foil m_BufferFoil;          /**< used for screen drawing and temporary geometric mods */
	Polar* m_pCurPolar;         /**< pointer to the currently selected foil polar */
	OpPoint* m_pCurOpp;         /**< pointer to the currently selected foil operating point */

	QList<void*> *m_poaFoil;	/**< pointer to the foil object array */
	QList<void*> *m_poaPolar;	/**< pointer to the polar object array */
	QList<void*> *m_poaOpp;		/**< pointer to the OpPoint object array */

	QGraph* m_pCpGraph;         /**< a pointer to the Cp graph for the OpPoint view */
	QGraph m_PlrGraph[MAXPOLARGRAPHS];          /**< the 5 Polar graphs */


	QGraph* m_pCurGraph;        /**< a pointer to the graph over which the mouse is hovering */

	QFile m_XFile;		        /**< The instance of the log file to which the text output of the analysis is directed */

	static void *s_pMainFrame;  /**< a pointer to the instance of the application's MainFrame object */
	static void *s_p2DWidget;   /**< a pointer to the instance of the application's central widget used for 2D drawings */

	QColor m_crBLColor;         /**< the color used to draw the boundary layer */
	QColor m_crPressureColor;   /**< the color used to draw the pressure arrows */
	QColor m_crNeutralColor;    /**< the color used to draw the neutral line */
	QColor m_CurveColor;        /**< the color displayed in the comboboxes for the selection of curve styles */

	int m_iBLStyle;             /**< the index of the style used to draw the boundary layer */
	int m_iBLWidth;             /**< the width of the line used to draw the boundary layer */
	int m_iPressureStyle;       /**< the index of the style used to draw the pressure arrows*/
	int m_iPressureWidth;       /**< the width of the line used to draw the pressure arrows */
	int m_iNeutralStyle;        /**< the index of the style used to draw the neutral line */
	int m_iNeutralWidth;        /**< the width of the line used to draw the neutral line */

	int m_CurveStyle;           /**< the index of the style of the lines displayed in the comboboxes for the selection of curve styles*/
	int m_CurveWidth;           /**< the width of the lines displayed in the comboboxes for the selection of curve styles*/
	
	QRect m_rCltRect;		    /**< the client rectangle of the central TwoDWidget */
	QPoint m_PointDown;		    /**< the client coordinated of the last mouse left-click */
	QPoint m_FoilOffset;		/**< the offset position for the foil display in the client area */
	QPoint m_PolarLegendOffset; /**< the offset position for the legend display in the client area */

	XFoil *m_pXFoil;		    /**< a pointer to the unique instance of the XFoil object */
};


#endif // QXDIRECT_H
