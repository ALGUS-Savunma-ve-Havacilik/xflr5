/****************************************************************************

	StabViewDlg Class
	Copyright (C) 2010 Andre Deperrois XFLR5@yahoo.com

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

#include "../Globals.h"
#include "../MainFrame.h"
#include "StabViewDlg.h"
#include "Miarex.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QtDebug>
#include <QTimer>
#include <complex>

#define MODEPOINTS            100


using namespace std;

void *StabViewDlg::s_pMiarex;
void *StabViewDlg::s_pMainFrame;


StabViewDlg::StabViewDlg(QWidget *parent)
{
	setWindowTitle(tr("Stability View Params"));
	setWindowFlags(Qt::Tool);
	m_iCurrentMode = 0;
	m_ModeAmplitude = 1.0;
	SetupLayout();
	Connect();
}


void StabViewDlg::showEvent(QShowEvent *event)
{
	SetControls();
}


void StabViewDlg::Connect()
{
	connect(m_pctrlPlotStabGraph, SIGNAL(clicked()), this , SLOT(OnPlotStabilityGraph()));

	connect(m_pctrlRLMode1, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlRLMode2, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlRLMode3, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlRLMode4, SIGNAL(clicked()), this, SLOT(OnModeSelection()));
	connect(m_pctrlAnimate ,SIGNAL(clicked()), this, SLOT(OnAnimate()));
	connect(m_pctrlAnimationSpeed ,SIGNAL(sliderMoved(int)), this, SLOT(OnAnimationSpeed(int)));
	connect(m_pctrlAnimationAmplitude ,SIGNAL(sliderMoved(int)), this, SLOT(OnAnimationAmplitude(int)));
	connect(m_pctrlAnimateRestart ,SIGNAL(clicked()), this, SLOT(OnAnimateRestart()));
	connect(m_pctrlDeltat, SIGNAL(editingFinished()), this, SLOT(OnReadData()));
	connect(m_pctrlModeStep, SIGNAL(editingFinished()), this, SLOT(OnReadData()));
	connect(m_pCtrlDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	connect(m_pctrlInitCondResponse, SIGNAL(clicked()), this, SLOT(OnResponseType()));
	connect(m_pctrlForcedResponse, SIGNAL(clicked()), this, SLOT(OnResponseType()));
	
	connect(m_pctrlKeepCurve, SIGNAL(clicked()), this, SLOT(OnKeepCurve()));
	connect(m_pctrlResetCurve, SIGNAL(clicked()), this, SLOT(OnResetCurve()));
	
	m_pControlModel = new QStandardItemModel;
	m_pControlModel->setRowCount(5);//temporary
	m_pControlModel->setColumnCount(2);
	m_pControlModel->setHeaderData(0, Qt::Horizontal, tr("Control Name"));
	m_pControlModel->setHeaderData(1, Qt::Horizontal, tr("Ramp ")+QString::fromUtf8("(°)"));
//	m_pControlModel->setHeaderData(2, Qt::Horizontal, tr("Ramp (s)"));

	m_pctrlControlTable->setModel(m_pControlModel);
	m_pctrlControlTable->setWindowTitle(tr("Controls"));
	m_pctrlControlTable->setColumnWidth(0,75);
	m_pctrlControlTable->setColumnWidth(1,60);
//	m_pctrlControlTable->setColumnWidth(2,60);
	QHeaderView *HorizontalHeader = m_pctrlControlTable->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);	
}


void StabViewDlg::FillControlNames()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CWPolar *pWPolar = pMiarex->m_pCurWPolar;
	QModelIndex ind;
	int i;
	QString strong;
	CPlane *pPlane = pMiarex->m_pCurPlane;
	CWing  *pWing  = pMiarex->m_pCurWing;
	CWing  *pStab  = pMiarex->m_pCurStab;
	CWing  *pFin   = pMiarex->m_pCurFin;
	
	if(!pWPolar || pWPolar->m_Type!=7) return;

	m_pControlModel->setRowCount(pWPolar->m_nControls);
	
	int nControls = 0;
	if(pPlane)
	{
		ind = m_pControlModel->index(nControls, 0, QModelIndex());
		m_pControlModel->setData(ind, tr("Wing Tilt (")+QString::fromUtf8("°")+")");
		++nControls;
		
		if(pStab)
		{
			ind = m_pControlModel->index(nControls, 0, QModelIndex());
			m_pControlModel->setData(ind, tr("Elevator Tilt ")+QString::fromUtf8("(°)"));
			++nControls;
		}
	}
	for(i=0; i<pWing->m_nFlaps; i++)
	{
		ind = m_pControlModel->index(i+nControls, 0, QModelIndex());
		strong = QString(tr("Wing Flap angle %1 ")+QString::fromUtf8("(°)")).arg(i+1);
		m_pControlModel->setData(ind, strong);
	}
	nControls += pWing->m_nFlaps;


	if(pStab)
	{
		for(i=0; i<pStab->m_nFlaps; i++)
		{
			ind = m_pControlModel->index(i+nControls, 0, QModelIndex());
			strong = QString(tr("Elevator Flap %1 ")+QString::fromUtf8("(°)")).arg(i+1);
			m_pControlModel->setData(ind, strong);

		}
		nControls += pStab->m_nFlaps;
	}
	if(pFin)
	{
		for(i=0; i<pFin->m_nFlaps; i++)
		{
			ind = m_pControlModel->index(i+nControls, 0, QModelIndex());
			strong = QString(tr("Fin Flap %1 ")+QString::fromUtf8("(°)")).arg(i+1);
			m_pControlModel->setData(ind, strong);

		}
		nControls += pFin->m_nFlaps;
	}
}


void StabViewDlg::FillEigenThings()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	FillControlNames();
	
	if(pMiarex->m_pCurWing && pMiarex->m_pCurWOpp && pMiarex->m_pCurWOpp->m_AnalysisType==4)
	{
		complex<double> c;
		double OmegaN, Omega1, Dsi, Sigma1;
		double sum, prod;
		QString strange;
		//We normalize the mode before display and only for display purposes
		double u0, mac, span;
		complex<double> angle;

		u0   = pMiarex->m_pCurWOpp->m_QInf;
		mac  = pMiarex->m_pCurWing->m_MAChord;
		span = pMiarex->m_pCurWing->m_PlanformSpan;

		c = pMiarex->m_pCurWOpp->m_EigenValue[m_iCurrentMode];
		if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
		else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
		m_pctrlEigenValue->setText(strange);

		sum  = c.real() * 2.0;                          // is a real number
		prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
		OmegaN = abs(c.imag());
		Omega1 = sqrt(prod);
		Sigma1 = sum /2.0;
		if(prod > PRECISION) Dsi = -Sigma1/Omega1;
		else                 Dsi = 0.0;

		m_pctrlFreqN->SetValue(OmegaN/2.0/PI);
		m_pctrlFreq1->SetValue(Omega1/2.0/PI);
		m_pctrlSigma1->SetValue(Sigma1);
		m_pctrlDsi->SetValue(Dsi);

		if(pMiarex->m_bLongitudinal && pMiarex->m_pCurWOpp)
		{
			angle = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3];
			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][0]/u0;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector1->setText(strange);

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][1]/u0;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector2->setText(strange);

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][2]/(2.0*u0/mac);
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector3->setText(strange);

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3]/angle;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector4->setText(strange);
		}
		else if(!pMiarex->m_bLongitudinal && pMiarex->m_pCurWOpp)
		{
			angle = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3];

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][0]/u0;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector1->setText(strange);

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][1]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector2->setText(strange);

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][2]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector3->setText(strange);

			c = pMiarex->m_pCurWOpp->m_EigenVector[m_iCurrentMode][3]/angle;
			if(c.imag()>=0.0) strange = QString("%1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("%1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			m_pctrlEigenVector4->setText(strange);
		}
	}
	else
	{
		m_pctrlEigenValue->setText("");
		m_pctrlEigenVector1->setText("");
		m_pctrlEigenVector2->setText("");
		m_pctrlEigenVector3->setText("");
		m_pctrlEigenVector4->setText("");
	}
}


void StabViewDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!m_pctrlPlotStabGraph->hasFocus()) m_pctrlPlotStabGraph->setFocus();
			else OnPlotStabilityGraph();
			 
			break;
		}
		 case Qt::Key_Escape:
		 {
			 if(m_pctrlAnimate->isChecked()) m_pctrlAnimate->setChecked(false);
			 OnAnimate();
			 break;
		 }
		default:
			event->ignore();
	}
}


void StabViewDlg::OnAnimate()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_pctrlAnimate->isChecked())
	{
//		pMiarex->m_iView = WSTABVIEW;
		pMiarex->m_iStabilityView=3;
		pMiarex->SetControls();
		
		pMiarex->m_Modedt = m_pctrlModeStep->GetValue();
		pMiarex->m_bAnimateMode  = true;
		int speed = m_pctrlAnimationSpeed->value();
		pMiarex->m_pTimerMode->setInterval(400-speed);
		pMiarex->m_pTimerMode->start();
	}
	else
	{
		pMiarex->StopAnimate();
	}
}



void StabViewDlg::OnAnimationAmplitude(int val)
{
	m_ModeAmplitude = (double)val/500.0;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->OnAnimateModeSingle(false);
}


void StabViewDlg::OnAnimationSpeed(int val)
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->m_pTimerMode->setInterval(400-val);
}


void StabViewDlg::OnAnimateRestart()
{
	double sigma, s2, omega, o2, maxso;
	double norm1, norm2, theta_sum, psi_sum, ModeState[6];

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CWOpp *pWOpp = pMiarex->m_pCurWOpp;
	CWing *pWing = pMiarex->m_pCurWing;

	pMiarex->m_ModeTime = 0.0;

	if(!pWOpp || !pWing)
	{
		pMiarex->m_ModeState[0] = 0.0;
		pMiarex->m_ModeState[1] = 0.0;
		pMiarex->m_ModeState[2] = 0.0;
		pMiarex->m_ModeState[3] = 0.0;
		pMiarex->m_ModeState[4] = 0.0;
		pMiarex->m_ModeState[5] = 0.0;
		pMiarex->UpdateView();
		return;
	}

	sigma = pWOpp->m_EigenValue[m_iCurrentMode].real();
	omega = pWOpp->m_EigenValue[m_iCurrentMode].imag();
	s2 = sigma*sigma;
	o2 = omega*omega;
	maxso  = qMax(fabs(sigma), fabs(omega));

//	if(abs(omega)>PRECISION)  Period =  2.0*PI/abs(omega);
//	else if(sigma>0.0)        Period =  1.0/abs(sigma);
//	else                      Period =  1.0/abs(sigma);
//	pMiarex->m_Modedt = Period/MODEPOINTS;

	m_vabs[0] = abs(pWOpp->m_EigenVector[m_iCurrentMode][0]);
	m_vabs[1] = abs(pWOpp->m_EigenVector[m_iCurrentMode][1]);
	m_vabs[2] = abs(pWOpp->m_EigenVector[m_iCurrentMode][2]);
	m_vabs[3] = abs(pWOpp->m_EigenVector[m_iCurrentMode][3]);
	m_phi[0]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][0]);
	m_phi[1]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][1]);
	m_phi[2]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][2]);
	m_phi[3]  = arg(pWOpp->m_EigenVector[m_iCurrentMode][3]);

	//calculate state at t=0 for normalization
	if(pMiarex->m_bLongitudinal)
	{
		//x, z, theta are evaluated by direct inegration of u, w, q
		ModeState[1] = 0.0;
		ModeState[3] = 0.0;
		ModeState[5] = 0.0;
		ModeState[0] = m_vabs[0]/(s2+o2) * (sigma*cos(m_phi[0])+omega*sin(m_phi[0]));
		ModeState[2] = m_vabs[1]/(s2+o2) * (sigma*cos(m_phi[1])+omega*sin(m_phi[1]));
		ModeState[4] = m_vabs[2]/(s2+o2) * (sigma*cos(m_phi[2])+omega*sin(m_phi[2]));
//		ModeState[4] = m_vabs[3]*cos(m_phi[3]);

		//add u0 x theta_sum to z component
		theta_sum      = m_vabs[3]/(s2+o2) * (sigma*cos(m_phi[3])+omega*sin(m_phi[3]));

		ModeState[2] -= theta_sum *pWOpp->m_QInf;
	}
	else
	{
		//y, phi, psi evaluation
		ModeState[0] = 0.0;
		ModeState[2] = 0.0;
		ModeState[4] = 0.0;

		// integrate (v+u0.psi.cos(theta0)) to get y
		ModeState[1] = m_vabs[0]/(s2+o2) * (sigma*cos(m_phi[0])+omega*sin(m_phi[0]));

		//integrate psi = integrate twice r (thanks Matlab !)
		psi_sum =   sigma * ( sigma * cos(m_phi[2]) + omega * sin(m_phi[2]))
				  + omega * (-omega * cos(m_phi[2]) + sigma * sin(m_phi[2]));
		psi_sum *= m_vabs[2]/ (s2+o2)/(s2+o2);

		ModeState[1] += pWOpp->m_QInf * psi_sum;

		// get directly phi from fourth eigenvector component (alternatively integrate p+r.tan(theta0));
		ModeState[3] = m_vabs[3]*cos(m_phi[3]);
//		m_ModeState[3] = m_ModeNorm*m_vabs[1]/(s2+o2) * (sigma*cos(m_phi[1])+omega*sin(m_phi[1]));

		// integrate once 'p+r.sin(theta0)' to get heading angle
		ModeState[5] = m_vabs[2]/(s2+o2) * (sigma*cos(m_phi[2])+omega*sin(m_phi[2]));
	}
	//max 10% span
	norm1 = qMax(fabs(ModeState[0]), fabs(ModeState[1]));
	norm1 = qMax(norm1, fabs(ModeState[2]));
	if(norm1>0.0)  norm1 = pWing->m_PlanformSpan *.10 / norm1;
	else           norm1 = 1.0;

	//max 10degrees
	norm2 = qMax(fabs(ModeState[3]), fabs(ModeState[4]));
	norm2 = qMax(norm2, fabs(ModeState[5]));
	if(norm2>0.0)  norm2 = PI*(10.0/180.0)/ norm2;
	else           norm2 = 1.0;

	pMiarex->m_ModeNorm = qMin(norm1, norm2);

	//set initial mode positions, i.e. t=0
	pMiarex->OnAnimateModeSingle(false);
//	pMiarex->m_ModeTime = 0.0;
//	pMiarex->UpdateView();
}


void StabViewDlg::OnCellChanged(QWidget *pWidget)
{
	
}


void StabViewDlg::OnKeepCurve()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CCurve *pCurve, *pNewCurve;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QColor CurveColor = pMainFrame->m_crColors[(pMiarex->m_TimeGraph1.GetCurveCount()+1)%24];

	pCurve = pMiarex->m_TimeGraph1.GetCurve(0);
	pNewCurve = pMiarex->m_TimeGraph1.AddCurve();
	pNewCurve->Copy(pCurve);
	pNewCurve->SetColor(CurveColor);

	pCurve = pMiarex->m_TimeGraph2.GetCurve(0);
	pNewCurve = pMiarex->m_TimeGraph2.AddCurve();
	pNewCurve->Copy(pCurve);
	pNewCurve->SetColor(CurveColor);

	pCurve = pMiarex->m_TimeGraph3.GetCurve(0);
	pNewCurve = pMiarex->m_TimeGraph3.AddCurve();
	pNewCurve->Copy(pCurve);
	pNewCurve->SetColor(CurveColor);

	pCurve = pMiarex->m_TimeGraph4.GetCurve(0);
	pNewCurve = pMiarex->m_TimeGraph4.AddCurve();
	pNewCurve->Copy(pCurve);
	pNewCurve->SetColor(CurveColor);

	QString str = QString( "%1").arg(pMiarex->m_TimeGraph1.GetCurveCount());
	str = tr("Curve ") + str;
	m_pctrlCurveTitle->setText(str);	
	
	pMiarex->SetCurveParams();
	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
}


void StabViewDlg::OnResetCurve()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	for(int i=pMiarex->m_TimeGraph1.GetCurveCount()-1; i>0 ; i--)
	{
		pMiarex->m_TimeGraph1.DeleteCurve(i);
		pMiarex->m_TimeGraph2.DeleteCurve(i);
		pMiarex->m_TimeGraph3.DeleteCurve(i);
		pMiarex->m_TimeGraph4.DeleteCurve(i);
	}
	pMiarex->SetCurveParams();
	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
}


void StabViewDlg::OnPlotStabilityGraph()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
	pMiarex->setFocus();
}


void StabViewDlg::OnModeSelection()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	if(m_pctrlRLMode1->isChecked())      m_iCurrentMode = 0;
	else if(m_pctrlRLMode2->isChecked()) m_iCurrentMode = 1;
	else if(m_pctrlRLMode3->isChecked()) m_iCurrentMode = 2;
	else if(m_pctrlRLMode4->isChecked()) m_iCurrentMode = 3;

	if(!pMiarex->m_bLongitudinal) m_iCurrentMode +=4;
	SetMode(m_iCurrentMode);
}


void StabViewDlg::OnReadData()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->m_Modedt = m_pctrlModeStep->GetValue();
	pMiarex->m_Deltat = m_pctrlDeltat->GetValue();
}



void StabViewDlg::OnResponseType()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->m_bForcedResponse = m_pctrlForcedResponse->isChecked();
	SetControls();
	pMiarex->CreateStabilityCurves();
	pMiarex->UpdateView();
	
}


void StabViewDlg::ReadForcedInput(double *in)
{
	int i;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CWPolar *pWPolar = pMiarex->m_pCurWPolar;

	for(i=0; i<pWPolar->m_nControls; i++)
	{
		in[i]    = m_pControlModel->index(i, 1, QModelIndex()).data().toDouble();
	}
}



void StabViewDlg::SetMode(int iMode)
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	if(iMode>0) m_iCurrentMode = iMode%4;
	else        m_iCurrentMode = 0;
	
	if(!pMiarex->m_bLongitudinal) m_iCurrentMode += 4;
	
	m_pctrlRLMode1->setChecked(m_iCurrentMode%4==0);
	m_pctrlRLMode2->setChecked(m_iCurrentMode%4==1);
	m_pctrlRLMode3->setChecked(m_iCurrentMode%4==2);
	m_pctrlRLMode4->setChecked(m_iCurrentMode%4==3);

	FillEigenThings();
	pMiarex->m_bResetglModeLegend = true;

	if(pMiarex->m_pCurRLStabGraph && pMiarex->m_pCurWPolar)
	{
		pMiarex->m_pCurRLStabGraph->DeselectPoint();
//		suffix = QString("_Mode_%1").arg(m_iCurrentMode+1);
//		str = pMiarex->m_pCurWPolar->m_PlrName+suffix;
//		bool bRes = pMiarex->m_pCurRLStabGraph->SelectPoint(str, m_iCurrentMode);
	}

	OnAnimateRestart();
}


void StabViewDlg::SetupLayout()
{
	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);
	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	//_______________________Time view Parameters
	QVBoxLayout *ResponseTypeLayout = new QVBoxLayout;
	m_pctrlInitCondResponse = new QRadioButton(tr("Initial Conditions Response"));
	m_pctrlForcedResponse = new QRadioButton(tr("Forced Response"));
	ResponseTypeLayout->addWidget(m_pctrlInitCondResponse);
	ResponseTypeLayout->addWidget(m_pctrlForcedResponse);
	
	QVBoxLayout	 *TimeParams = new QVBoxLayout;
	m_pctrlStabLabel1 = new QLabel("u0__");
	m_pctrlStabLabel2 = new QLabel("w0__");
	m_pctrlStabLabel3 = new QLabel("q0__");
	m_pctrlUnit1 = new QLabel("m/s");
	m_pctrlUnit2 = new QLabel("m/s");
	m_pctrlUnit3 = new QLabel("rad/s");
	m_pctrlStabLabel1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabLabel2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabLabel3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlUnit1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pctrlUnit2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pctrlUnit3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pctrlStabVar1 = new FloatEdit(0.00,3);
	m_pctrlStabVar2 = new FloatEdit(0.00,3);
	m_pctrlStabVar3 = new FloatEdit(1.00,3);
	QGridLayout *VarParams = new QGridLayout;
	VarParams->addWidget(m_pctrlStabLabel1,1,1);
	VarParams->addWidget(m_pctrlStabLabel2,2,1);
	VarParams->addWidget(m_pctrlStabLabel3,3,1);
	VarParams->addWidget(m_pctrlStabVar1,1,2);
	VarParams->addWidget(m_pctrlStabVar2,2,2);
	VarParams->addWidget(m_pctrlStabVar3,3,2);
	VarParams->addWidget(m_pctrlUnit1,1,3);
	VarParams->addWidget(m_pctrlUnit2,2,3);
	VarParams->addWidget(m_pctrlUnit3,3,3);
	QGroupBox *InitCondResponse = new QGroupBox(tr("Initial conditions"));
	InitCondResponse->setLayout(VarParams);
	
	QGroupBox *ForcedResponseBox = new QGroupBox(tr("Forced Response"));
	QVBoxLayout *ForcedResponse = new QVBoxLayout;
	m_pctrlControlTable = new QTableView(this);
//	m_pctrlControlTable->setMinimumWidth(200);
	m_pctrlControlTable->setMinimumHeight(200);
//	m_pctrlControlTable->setSizePolicy(szPolicyMaximum);
	m_pctrlControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlControlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pCtrlDelegate = new CtrlTableDelegate;
	int  *precision = new int[6];
	precision[0]  = 0;
	precision[1]  = 3;
	precision[2]  = 3;
	precision[3]  = 3;
	precision[4]  = 3;
	precision[5]  = 3;
	m_pCtrlDelegate->m_Precision = precision;
	m_pctrlControlTable->setItemDelegate(m_pCtrlDelegate);
	m_pCtrlDelegate->m_pCtrlModel = m_pControlModel;
	ForcedResponse->addWidget(m_pctrlControlTable);
	ForcedResponseBox->setLayout(ForcedResponse);

	m_pctrlTotalTime = new FloatEdit(5,3);
	m_pctrlDeltat    = new FloatEdit(.01,3);
	m_pctrlRampTime  = new FloatEdit(0.1,3);
	

	QGridLayout *DtLayout  = new QGridLayout;
	QLabel *DtLabel        = new QLabel("dt=");
	QLabel *TotalTimeLabel = new QLabel(tr("Total Time="));
	QLabel *RampTimeLabel  = new QLabel(tr("Ramp Time="));
	QLabel *TimeLab1       = new QLabel("s");
	QLabel *TimeLab2       = new QLabel("s");
	QLabel *TimeLab3       = new QLabel("s");
	DtLayout->addWidget(DtLabel,1,1);
	DtLayout->addWidget(m_pctrlDeltat,1,2);
	DtLayout->addWidget(TimeLab1,1,3);
	DtLayout->addWidget(TotalTimeLabel,2,1);
	DtLayout->addWidget(m_pctrlTotalTime,2,2);
	DtLayout->addWidget(TimeLab2,2,3);
	DtLayout->addWidget(RampTimeLabel,3,1);
	DtLayout->addWidget(m_pctrlRampTime,3,2);
	DtLayout->addWidget(TimeLab3,3,3);

	m_pctrlPlotStabGraph = new QPushButton(tr("Plot"));
	QHBoxLayout *KeepCurve = new QHBoxLayout;
	m_pctrlKeepCurve  = new QPushButton(tr("Keep"));
	m_pctrlResetCurve = new QPushButton(tr("Reset"));
	m_pctrlCurveTitle = new QLineEdit(tr("Curve")+" 1");
	KeepCurve->addWidget(m_pctrlKeepCurve);
	KeepCurve->addWidget(m_pctrlResetCurve);
	TimeParams->addLayout(ResponseTypeLayout);
	TimeParams->addWidget(InitCondResponse);
	TimeParams->addStretch(1);
	TimeParams->addWidget(ForcedResponseBox);
	TimeParams->addStretch(1);
	TimeParams->addLayout(DtLayout);
	TimeParams->addWidget(m_pctrlCurveTitle);
	TimeParams->addWidget(m_pctrlPlotStabGraph);
	TimeParams->addLayout(KeepCurve);
	TimeParams->addStretch(5);
	QGroupBox *TimeBox = new QGroupBox(tr("Time Graph Params"));
	TimeBox->setLayout(TimeParams);
	//_______________________Root Locus View Parameters
	QVBoxLayout *RLLayout = new QVBoxLayout;
	QHBoxLayout *RLModeLayout = new QHBoxLayout;
	m_pctrlRLMode1 = new QRadioButton("1");
	m_pctrlRLMode2 = new QRadioButton("2");
	m_pctrlRLMode3 = new QRadioButton("3");
	m_pctrlRLMode4 = new QRadioButton("4");
	RLModeLayout->addWidget(m_pctrlRLMode1);
	RLModeLayout->addWidget(m_pctrlRLMode2);
	RLModeLayout->addWidget(m_pctrlRLMode3);
	RLModeLayout->addWidget(m_pctrlRLMode4);
	QGroupBox *RLModeBox = new QGroupBox(tr("Mode Selection"));
	RLModeBox->setLayout(RLModeLayout);

	QGroupBox * EigenBox = new QGroupBox(tr("Eigenvalues"));
	QGridLayout *EigenLayout = new QGridLayout;
	QLabel *LabValue = new QLabel("l=");
	QFont SymbolFont("Symbol");
	LabValue->setFont(SymbolFont);
	QLabel *LabVect1 = new QLabel("v1=");
	QLabel *LabVect2 = new QLabel("v2=");
	QLabel *LabVect3 = new QLabel("v3=");
	QLabel *LabVect4 = new QLabel("v4=");
	EigenLayout->addWidget(LabValue,1,1);
	EigenLayout->addWidget(LabVect1,2,1);
	EigenLayout->addWidget(LabVect2,3,1);
	EigenLayout->addWidget(LabVect3,4,1);
	EigenLayout->addWidget(LabVect4,5,1);
	m_pctrlEigenValue = new QLineEdit("2+4i");
	m_pctrlEigenVector1 = new QLineEdit("3-7i");
	m_pctrlEigenVector2 = new QLineEdit("4-6i");
	m_pctrlEigenVector3 = new QLineEdit("2.76-1.8782i");
	m_pctrlEigenVector4 = new QLineEdit("3.4567+9.2746i");
	m_pctrlEigenValue->setAlignment(Qt::AlignRight);
	m_pctrlEigenVector1->setAlignment(Qt::AlignRight);
	m_pctrlEigenVector2->setAlignment(Qt::AlignRight);
	m_pctrlEigenVector3->setAlignment(Qt::AlignRight);
	m_pctrlEigenVector4->setAlignment(Qt::AlignRight);
	m_pctrlEigenValue->setEnabled(false);
	m_pctrlEigenVector1->setEnabled(false);
	m_pctrlEigenVector2->setEnabled(false);
	m_pctrlEigenVector3->setEnabled(false);
	m_pctrlEigenVector4->setEnabled(false);
	EigenLayout->addWidget(m_pctrlEigenValue,  1,2);
	EigenLayout->addWidget(m_pctrlEigenVector1,2,2);
	EigenLayout->addWidget(m_pctrlEigenVector2,3,2);
	EigenLayout->addWidget(m_pctrlEigenVector3,4,2);
	EigenLayout->addWidget(m_pctrlEigenVector4,5,2);
	QLabel *FreqNLab = new QLabel("F =");
	QLabel *Freq1Lab = new QLabel(tr("F1 ="));
	QLabel *SigmaLab = new QLabel(tr("s1 ="));
	QLabel *DsiLab   = new QLabel(tr("z ="));
	FreqNLab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	Freq1Lab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	SigmaLab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	DsiLab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	SigmaLab->setFont(SymbolFont);
	DsiLab->setFont(SymbolFont);

	m_pctrlFreqN  = new FloatEdit(0.0,3);
	m_pctrlFreq1  = new FloatEdit(0.0,3);
	m_pctrlSigma1 = new FloatEdit(0.0,3);
	m_pctrlDsi    = new FloatEdit(0.0,3);
	m_pctrlFreqN->setEnabled(false);
	m_pctrlFreq1->setEnabled(false);
	m_pctrlSigma1->setEnabled(false);
	m_pctrlDsi->setEnabled(false);
	QLabel *FreqUnit1 = new QLabel("Hz");
	QLabel *FreqUnit2 = new QLabel("Hz");
	QLabel *DampUnit1 = new QLabel("s-1");
	QGridLayout *FreakLayout = new QGridLayout;
	FreakLayout->addWidget(FreqNLab,1,1);
	FreakLayout->addWidget(Freq1Lab,2,1);
	FreakLayout->addWidget(SigmaLab,3,1);
	FreakLayout->addWidget(DsiLab,4,1);
	FreakLayout->addWidget(m_pctrlFreqN,1,2);
	FreakLayout->addWidget(m_pctrlFreq1,2,2);
	FreakLayout->addWidget(m_pctrlSigma1,3,2);
	FreakLayout->addWidget(m_pctrlDsi,4,2);
	FreakLayout->addWidget(FreqUnit1,1,3);
	FreakLayout->addWidget(FreqUnit2,2,3);
	FreakLayout->addWidget(DampUnit1,3,3);
	QGroupBox *FreakBox = new QGroupBox(tr("Mode properties"));
	FreakBox->setLayout(FreakLayout);


	QVBoxLayout *AnimationLayout = new QVBoxLayout;

	QGridLayout *AnimSpeedLayout = new QGridLayout;
	QLabel *LabSpeed = new QLabel(tr("Speed"));
	LabSpeed->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
	m_pctrlAnimationSpeed  = new QDial();
	m_pctrlAnimationSpeed->setMinimum(0);
	m_pctrlAnimationSpeed->setMaximum(400);
	m_pctrlAnimationSpeed->setSliderPosition(200);
	m_pctrlAnimationSpeed->setNotchesVisible(true);
	m_pctrlAnimationSpeed->setSingleStep(20);
//	m_pctrlAnimationSpeed->setTickInterval(50);
//	m_pctrlAnimationSpeed->setTickPosition(QSlider::TicksBelow);
//	m_pctrlAnimationSpeed->setSizePolicy(szPolicyMinimum);
	AnimSpeedLayout->addWidget(m_pctrlAnimationSpeed,1,1);
	AnimSpeedLayout->addWidget(LabSpeed,2,1);

	QLabel *LabAmplitude = new QLabel(tr("Amplitude"));
	LabAmplitude->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
	m_pctrlAnimationAmplitude  = new QDial();
	m_pctrlAnimationAmplitude->setMinimum(0);
	m_pctrlAnimationAmplitude->setMaximum(1000);
	m_pctrlAnimationAmplitude->setSliderPosition((int)(m_ModeAmplitude*500));
	m_pctrlAnimationAmplitude->setNotchesVisible(true);
	m_pctrlAnimationAmplitude->setSingleStep(20);
//	m_pctrlAnimationAmplitude->setTickInterval(50);
//	m_pctrlAnimationAmplitude->setTickPosition(QSlider::TicksBelow);
//	m_pctrlAnimationAmplitude->setSizePolicy(szPolicyMinimum);
	AnimSpeedLayout->addWidget(m_pctrlAnimationAmplitude,1,2);
	AnimSpeedLayout->addWidget(LabAmplitude,2,2);

	m_pctrlAnimate = new QPushButton(tr("Animate"));
	m_pctrlAnimate->setCheckable(true);
	m_pctrlAnimateRestart = new QPushButton(tr("Restart"));
	m_pctrlModeStep = new FloatEdit(0.01,3);
	QLabel *StepLabel = new QLabel(tr("Time Step ="));
	QLabel *StepUnit  = new QLabel(tr("s"));
	QHBoxLayout *StepLayout = new  QHBoxLayout;
	StepLayout->addWidget(StepLabel);
	StepLayout->addWidget(m_pctrlModeStep);
	StepLayout->addWidget(StepUnit);
	AnimationLayout->addLayout(StepLayout);
	AnimationLayout->addStretch(1);
	AnimationLayout->addLayout(AnimSpeedLayout);
//	AnimationLayout->addLayout(AnimAmplitudeLayout);
	AnimationLayout->addStretch(1);
	AnimationLayout->addWidget(m_pctrlAnimateRestart);
	AnimationLayout->addStretch(1);
	AnimationLayout->addWidget(m_pctrlAnimate);
	AnimationLayout->addStretch(3);
	m_pctrlAnimationBox = new QGroupBox(tr("Animation"));
	m_pctrlAnimationBox->setLayout(AnimationLayout);


	EigenBox->setLayout(EigenLayout);
	RLLayout->addWidget(RLModeBox);
	RLLayout->addWidget(EigenBox);
	RLLayout->addWidget(FreakBox);
	RLLayout->addWidget(m_pctrlAnimationBox);
	RLLayout->addStretch(1);
	QGroupBox *RLBox = new QGroupBox(tr("Mode"));
	RLBox->setLayout(RLLayout);


	m_pctrlStackWidget = new QStackedWidget;
	m_pctrlStackWidget->addWidget(TimeBox);
	m_pctrlStackWidget->addWidget(RLBox);
	m_pctrlStackWidget->setCurrentIndex(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pctrlStackWidget);
	MainLayout->addStretch(1);
	setLayout(MainLayout);
}


void StabViewDlg::SetControls()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	QString str;
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);

	m_pctrlRLMode1->setChecked(m_iCurrentMode%4==0);
	m_pctrlRLMode2->setChecked(m_iCurrentMode%4==1);
	m_pctrlRLMode3->setChecked(m_iCurrentMode%4==2);
	m_pctrlRLMode4->setChecked(m_iCurrentMode%4==3);

	if(pMiarex->m_pCurWPolar && pMiarex->m_pCurWPolar->m_Type!=7)
	{
		m_pControlModel->setRowCount(0);		
	}
	if(pMiarex->m_iStabilityView==0)
	{
		m_pctrlStackWidget->setCurrentIndex(0);
		m_pctrlForcedResponse->setChecked(pMiarex->m_bForcedResponse);
		m_pctrlInitCondResponse->setChecked(!pMiarex->m_bForcedResponse);
		m_pctrlStabVar1->setEnabled(!pMiarex->m_bForcedResponse);
		m_pctrlStabVar2->setEnabled(!pMiarex->m_bForcedResponse);
		m_pctrlStabVar3->setEnabled(!pMiarex->m_bForcedResponse);
		m_pctrlControlTable->setEnabled(pMiarex->m_bForcedResponse);
		m_pctrlRampTime->setEnabled(pMiarex->m_bForcedResponse);
	}
	else if(pMiarex->m_iStabilityView==1 || pMiarex->m_iStabilityView==3 || pMiarex->m_iView==2)
	{
		m_pctrlStackWidget->setCurrentIndex(1);
		if(pMiarex->m_iStabilityView==3) m_pctrlAnimationBox->show();
		else                             m_pctrlAnimationBox->hide();
		SetMode(m_iCurrentMode);
	}

	if(pMiarex->m_bLongitudinal)
	{
		m_pctrlStabLabel1->setText(tr("u0="));
		m_pctrlStabLabel2->setText(tr("w0="));
		m_pctrlStabLabel3->setText(tr("q0="));
		m_pctrlUnit1->setText(str);
		m_pctrlUnit2->setText(str);
		m_pctrlUnit3->setText("rad/s");
	}
	else
	{
		m_pctrlStabLabel1->setText(tr("v0="));
		m_pctrlStabLabel2->setText(tr("p0="));
		m_pctrlStabLabel3->setText(tr("r0="));
		m_pctrlUnit1->setText(str);
		m_pctrlUnit2->setText("rad/s");
		m_pctrlUnit3->setText("rad/s");
	}

	m_pctrlStabVar1->SetValue(pMiarex->m_TimeInput[0]);
	m_pctrlStabVar2->SetValue(pMiarex->m_TimeInput[1]);
	m_pctrlStabVar3->SetValue(pMiarex->m_TimeInput[2]);
	m_pctrlTotalTime->SetValue(pMiarex->m_TotalTime);
	m_pctrlDeltat->SetValue(pMiarex->m_Deltat);

	m_pctrlRLMode1->setEnabled(pMiarex->m_iStabilityView>0);
	m_pctrlRLMode2->setEnabled(pMiarex->m_iStabilityView>0);
	m_pctrlRLMode3->setEnabled(pMiarex->m_iStabilityView>0);
	m_pctrlRLMode4->setEnabled(pMiarex->m_iStabilityView>0);
	
	m_pctrlModeStep->SetValue(pMiarex->m_Modedt);
	FillEigenThings();
}

