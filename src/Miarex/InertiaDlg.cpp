/****************************************************************************

	InertiaDlg Class
	Copyright (C) 2009-2010 Andre Deperrois XFLR5@yahoo.com

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


#include "../MainFrame.h"
#include "../Globals.h"
#include "InertiaDlg.h"
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextStream>
#include <QFileDialog>
#include <QtDebug>



InertiaDlg::InertiaDlg()
{
	setWindowTitle(tr("Inertia Properties"));

	m_pPlane = NULL;
	m_pWing = NULL;
	m_pBody = NULL;

	m_PtRef.Set(0.0, 0.0, 0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_NMass = 3;

	m_Mass = 1.0;

	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));

	for(int i=0; i< MAXMASSES; i++)
	{
		m_MassTag[i] = QString(tr("Description %1")).arg(i);
	}

	m_bChanged = false;

	SetupLayout();
}


void InertiaDlg::ComputeInertia()
{
	//assumes that the data has been read
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int i;
	double TotalMass, TotalIxx, TotalIyy, TotalIzz, TotalIxz;
	double Unit = pMainFrame->m_mtoUnit * pMainFrame->m_mtoUnit * pMainFrame->m_kgtoUnit;
	CVector TotalCoG;
	double MassValue[MAXMASSES];
	CVector MassPosition[MAXMASSES];

	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeCoG.Set(0.0, 0.0, 0.0);
	m_PtRef.Set(0.0, 0.0, 0.0);

	//First evaluate the object's inertia

	if(m_pWing)
	{
		m_pWing->ComputeVolumeInertia(m_Mass, m_VolumeCoG, m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz);
	}
	else if(m_pBody)
	{
		m_pBody->ComputeBodyInertia(m_Mass, m_VolumeCoG, m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz);
	}
	else if(m_pPlane)
	{
		m_pPlane->ComputeVolumeInertia(m_Mass, m_VolumeCoG, m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz);
	}

	//and display the results

	//Volume masses
	m_pctrlXCoG->SetValue(m_VolumeCoG.x*pMainFrame->m_mtoUnit);
	m_pctrlYCoG->SetValue(m_VolumeCoG.y*pMainFrame->m_mtoUnit);
	m_pctrlZCoG->SetValue(m_VolumeCoG.z*pMainFrame->m_mtoUnit);

	m_pctrlCoGIxx->SetValue(m_CoGIxx*Unit);
	m_pctrlCoGIyy->SetValue(m_CoGIyy*Unit);
	m_pctrlCoGIzz->SetValue(m_CoGIzz*Unit);
	m_pctrlCoGIxz->SetValue(m_CoGIxz*Unit);
	

	//_______________________________________________
	//Total masses = volume + point masses of all elements part of the object
	
	TotalCoG.Set(m_Mass*m_VolumeCoG.x, m_Mass*m_VolumeCoG.y, m_Mass*m_VolumeCoG.z);
	TotalMass = m_Mass;
	TotalIxx = TotalIyy = TotalIzz = TotalIxz = 0.0;
//
	for(i=0; i<m_NMass; i++)
	{
		TotalMass += m_MassValue[i];
		TotalCoG  += m_MassPosition[i] * m_MassValue[i];
	}
	if(m_pPlane)
	{
		for(i=0; i<m_pPlane->m_Wing.m_NMass; i++)
		{
			TotalMass += m_pPlane->m_Wing.m_MassValue[i];
			TotalCoG  += m_pPlane->m_Wing.m_MassPosition[i] * m_pPlane->m_Wing.m_MassValue[i];
		}
		if(m_pPlane->m_bBiplane)
		{
			for(i=0; i<m_pPlane->m_Wing2.m_NMass; i++)
			{
				TotalMass += m_pPlane->m_Wing2.m_MassValue[i];
				TotalCoG  += m_pPlane->m_Wing2.m_MassPosition[i] * m_pPlane->m_Wing2.m_MassValue[i];
			}
		}
		if(m_pPlane->m_bStab)
		{
			for(i=0; i<m_pPlane->m_Stab.m_NMass; i++)
			{
				TotalMass += m_pPlane->m_Stab.m_MassValue[i];
				TotalCoG  += m_pPlane->m_Stab.m_MassPosition[i] * m_pPlane->m_Stab.m_MassValue[i];
			}
		}
		if(m_pPlane->m_bFin)
		{
			for(i=0; i<m_pPlane->m_Fin.m_NMass; i++)
			{
				TotalMass += m_pPlane->m_Fin.m_MassValue[i];
				TotalCoG  += m_pPlane->m_Fin.m_MassPosition[i] * m_pPlane->m_Fin.m_MassValue[i];
			}
		}
		if(m_pPlane->m_bBody)
		{
			for(i=0; i<m_pPlane->m_pBody->m_NMass; i++)
			{
				TotalMass += m_pPlane->m_pBody->m_MassValue[i];
				TotalCoG  += m_pPlane->m_pBody->m_MassPosition[i] * m_pPlane->m_pBody->m_MassValue[i];
			}
		}
	}
	TotalCoG *= 1.0/TotalMass;

	//total inertia in CoG referential
	TotalIxx = m_CoGIxx + m_Mass * ((m_VolumeCoG.y-TotalCoG.y)*(m_VolumeCoG.y-TotalCoG.y)
								   +(m_VolumeCoG.z-TotalCoG.z)*(m_VolumeCoG.z-TotalCoG.z));
	TotalIyy = m_CoGIyy + m_Mass * ((m_VolumeCoG.x-TotalCoG.x)*(m_VolumeCoG.x-TotalCoG.x)
								   +(m_VolumeCoG.z-TotalCoG.z)*(m_VolumeCoG.z-TotalCoG.z));
	TotalIzz = m_CoGIzz + m_Mass * ((m_VolumeCoG.x-TotalCoG.x)*(m_VolumeCoG.x-TotalCoG.x)
								   +(m_VolumeCoG.y-TotalCoG.y)*(m_VolumeCoG.y-TotalCoG.y));
	TotalIxz = m_CoGIxz - m_Mass *  (m_VolumeCoG.x-TotalCoG.x)*(m_VolumeCoG.z-TotalCoG.z) ;

	for(i=0; i<m_NMass; i++)
	{
		TotalIxx  += m_MassValue[i] * ((m_MassPosition[i].y-TotalCoG.y)*(m_MassPosition[i].y-TotalCoG.y)
									  +(m_MassPosition[i].z-TotalCoG.z)*(m_MassPosition[i].z-TotalCoG.z));
		TotalIyy  += m_MassValue[i] * ((m_MassPosition[i].x-TotalCoG.x)*(m_MassPosition[i].x-TotalCoG.x)
									  +(m_MassPosition[i].z-TotalCoG.z)*(m_MassPosition[i].z-TotalCoG.z));
		TotalIzz  += m_MassValue[i] * ((m_MassPosition[i].x-TotalCoG.x)*(m_MassPosition[i].x-TotalCoG.x)
									  +(m_MassPosition[i].y-TotalCoG.y)*(m_MassPosition[i].y-TotalCoG.y));
		TotalIxz  -= m_MassValue[i] *  (m_MassPosition[i].x-TotalCoG.x)*(m_MassPosition[i].z-TotalCoG.z) ;
	}

	if(m_pPlane)
	{
		memcpy(MassValue,    m_pPlane->m_Wing.m_MassValue,      MAXMASSES*sizeof(double));
		memcpy(MassPosition, m_pPlane->m_Wing.m_MassPosition, 3*MAXMASSES*sizeof(double));
		for(i=0; i<m_pPlane->m_Wing.m_NMass; i++)
		{
			TotalIxx  += MassValue[i] * ((MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y)
										+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
			TotalIyy  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
										+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
			TotalIzz  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
										+(MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y));
			TotalIxz  -= MassValue[i] *  (MassPosition[i].x-TotalCoG.x)*(MassPosition[i].z-TotalCoG.z) ;
		}
		if(m_pPlane->m_bBiplane)
		{
			memcpy(MassValue,    m_pPlane->m_Wing2.m_MassValue,      MAXMASSES*sizeof(double));
			memcpy(MassPosition, m_pPlane->m_Wing2.m_MassPosition, 3*MAXMASSES*sizeof(double));
			for(i=0; i<m_pPlane->m_Wing2.m_NMass; i++)
			{
				TotalIxx  += MassValue[i] * ((MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIyy  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIzz  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y));
				TotalIxz  -= MassValue[i] *  (MassPosition[i].x-TotalCoG.x)*(MassPosition[i].z-TotalCoG.z) ;
			}
		}
		if(m_pPlane->m_bStab)
		{
			memcpy(MassValue,    m_pPlane->m_Stab.m_MassValue,      MAXMASSES*sizeof(double));
			memcpy(MassPosition, m_pPlane->m_Stab.m_MassPosition, 3*MAXMASSES*sizeof(double));
			for(i=0; i<m_pPlane->m_Stab.m_NMass; i++)
			{
				TotalIxx  += MassValue[i] * ((MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIyy  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIzz  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y));
				TotalIxz  -= MassValue[i] *  (MassPosition[i].x-TotalCoG.x)*(MassPosition[i].z-TotalCoG.z) ;
			}
		}
		if(m_pPlane->m_bFin)
		{
			memcpy(MassValue,    m_pPlane->m_Fin.m_MassValue,      MAXMASSES*sizeof(double));
			memcpy(MassPosition, m_pPlane->m_Fin.m_MassPosition, 3*MAXMASSES*sizeof(double));
			for(i=0; i<m_pPlane->m_Fin.m_NMass; i++)
			{
				TotalIxx  += MassValue[i] * ((MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIyy  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIzz  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y));
				TotalIxz  -= MassValue[i] *  (MassPosition[i].x-TotalCoG.x)*(MassPosition[i].z-TotalCoG.z) ;
			}
		}
		if(m_pPlane->m_bBody)
		{
			memcpy(MassValue,    m_pPlane->m_pBody->m_MassValue,      MAXMASSES*sizeof(double));
			memcpy(MassPosition, m_pPlane->m_pBody->m_MassPosition, 3*MAXMASSES*sizeof(double));
			for(i=0; i<m_pPlane->m_pBody->m_NMass; i++)
			{
				TotalIxx  += MassValue[i] * ((MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIyy  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].z-TotalCoG.z)*(MassPosition[i].z-TotalCoG.z));
				TotalIzz  += MassValue[i] * ((MassPosition[i].x-TotalCoG.x)*(MassPosition[i].x-TotalCoG.x)
											+(MassPosition[i].y-TotalCoG.y)*(MassPosition[i].y-TotalCoG.y));
				TotalIxz  -= MassValue[i] *  (MassPosition[i].x-TotalCoG.x)*(MassPosition[i].z-TotalCoG.z) ;
			}
		}
	}

	m_pctrlTotalMass->SetValue(TotalMass*pMainFrame->m_kgtoUnit);
	
	m_pctrlXTotalCoG->SetValue(TotalCoG.x*pMainFrame->m_mtoUnit);
	m_pctrlYTotalCoG->SetValue(TotalCoG.y*pMainFrame->m_mtoUnit);
	m_pctrlZTotalCoG->SetValue(TotalCoG.z*pMainFrame->m_mtoUnit);

	m_pctrlTotalIxx->SetValue(TotalIxx*Unit);
	m_pctrlTotalIyy->SetValue(TotalIyy*Unit);
	m_pctrlTotalIzz->SetValue(TotalIzz*Unit);
	m_pctrlTotalIxz->SetValue(TotalIxz*Unit);

}


void InertiaDlg::FillMassModel()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QModelIndex index;
	
	m_pMassModel->setRowCount(MAXMASSES);
	for(int i=0; i<m_NMass; i++)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		m_pMassModel->setData(index, m_MassValue[i]*pMainFrame->m_kgtoUnit);

		if(m_MassValue[i]>0.0)
		{
			index = m_pMassModel->index(i, 1, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].x*pMainFrame->m_mtoUnit);

			index = m_pMassModel->index(i, 2, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].y*pMainFrame->m_mtoUnit);

			index = m_pMassModel->index(i, 3, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].z*pMainFrame->m_mtoUnit);

			index = m_pMassModel->index(i, 4, QModelIndex());
			m_pMassModel->setData(index, m_MassTag[i]);
		}
		else
		{
			index = m_pMassModel->index(i, 1, QModelIndex());
			m_pMassModel->setData(index, 0.0);

			index = m_pMassModel->index(i, 2, QModelIndex());
			m_pMassModel->setData(index, 0.0);

			index = m_pMassModel->index(i, 3, QModelIndex());
			m_pMassModel->setData(index, 0.0);

			index = m_pMassModel->index(i, 4, QModelIndex());
			m_pMassModel->setData(index, "");
		}
	}
}


void InertiaDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString str, str1, strong;

	m_pMassModel = new QStandardItemModel;
	m_pMassModel->setRowCount(10);//temporary
	m_pMassModel->setColumnCount(5);

	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pMassModel->setHeaderData(0, Qt::Horizontal, tr("Mass") +" ("+str+")");
	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pMassModel->setHeaderData(1, Qt::Horizontal, tr("x") +" ("+str+")");
	m_pMassModel->setHeaderData(2, Qt::Horizontal, tr("y")+" ("+str+")");
	m_pMassModel->setHeaderData(3, Qt::Horizontal, tr("z")+" ("+str+")");
	m_pMassModel->setHeaderData(4, Qt::Horizontal, tr("Description"));

	m_pctrlMassView->setModel(m_pMassModel);
	m_pctrlMassView->setColumnWidth(0,90);
	m_pctrlMassView->setColumnWidth(1,70);
	m_pctrlMassView->setColumnWidth(2,70);
	m_pctrlMassView->setColumnWidth(3,70);
	m_pctrlMassView->setColumnWidth(4,150);

	m_pFloatDelegate = new FloatEditDelegate;
	m_pctrlMassView->setItemDelegate(m_pFloatDelegate);
	int  *precision = new int[5];
	precision[0] = 3;
	precision[1] = 3;
	precision[2] = 3;
	precision[3] = 3;
	precision[4] = -1;// Not a number, will be detected as such by FloatEditDelegate
	m_pFloatDelegate->SetPrecision(precision);

	connect(m_pFloatDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pctrlMassUnit->setText(str);
	m_pctrlMassUnit2->setText(str);
	GetLengthUnit(str1, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit->setText(str1);
	m_pctrlLengthUnit2->setText(str1);
	strong = str+"."+str1+"2";
	m_pctrlInertiaUnit1->setText(strong);
	m_pctrlInertiaUnit2->setText(strong);

	if(m_pWing)
	{
		m_Mass = m_pWing->m_Mass;
		m_NMass = m_pWing->m_NMass;
		for(int i=0; i<m_pWing->m_NMass; i++)
		{
			m_MassValue[i] = m_pWing->m_MassValue[i];
			m_MassPosition[i].Copy(m_pWing->m_MassPosition[i]);
			m_MassTag[i]   = m_pWing->m_MassTag[i];
		}
		m_pctrlVolumeMass->SetValue(m_pWing->m_Mass * pMainFrame->m_kgtoUnit); //we only display half a wing, AVL way
		m_pctrlVolumeMassLabel->setText(tr("Wing Mass:"));
	}
	else if (m_pBody)
	{
		m_Mass = m_pBody->m_Mass;
		m_NMass = m_pBody->m_NMass;
		for(int i=0; i<m_pBody->m_NMass; i++)
		{
			m_MassValue[i] = m_pBody->m_MassValue[i];
			m_MassPosition[i].Copy(m_pBody->m_MassPosition[i]);
			m_MassTag[i]   = m_pBody->m_MassTag[i];
		}
		m_pctrlVolumeMass->SetValue(m_pBody->m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlVolumeMassLabel->setText(tr("Body Mass:"));
	}
	else if (m_pPlane)
	{
		m_Mass = m_pPlane->m_Wing.m_Mass;
		if(m_pPlane->m_bBiplane) m_Mass += m_pPlane->m_Wing2.m_Mass;
		if(m_pPlane->m_bStab)    m_Mass += m_pPlane->m_Stab.m_Mass;
		if(m_pPlane->m_bFin)     m_Mass += m_pPlane->m_Fin.m_Mass;
		if(m_pPlane->m_bBody && m_pPlane->m_pBody) m_Mass += m_pPlane->m_pBody->m_Mass;

		m_NMass = m_pPlane->m_NMass;
		for(int i=0; i<m_pPlane->m_NMass; i++)
		{
			m_MassValue[i] = m_pPlane->m_MassValue[i];
			m_MassPosition[i].Copy(m_pPlane->m_MassPosition[i]);
			m_MassTag[i]   = m_pPlane->m_MassTag[i];
		}

		m_pctrlVolumeMass->SetValue(m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlVolumeMassLabel->setText(tr("Volume Mass:"));
		m_pctrlVolumeMass->setEnabled(false);
	}

	FillMassModel();
	ComputeInertia();
	setFocus();
}


void InertiaDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus()) OKButton->setFocus();
			else                      accept();

			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		default:
			event->ignore();
	}
}


void InertiaDlg::OnCellChanged(QWidget *pWidget)
{
	ReadData();
	ComputeInertia();
	m_bChanged = true;
}


void InertiaDlg::OnExportToAVL()
{
	if (!m_pWing && !m_pBody && !m_pPlane) return;
	QString filter =".mass";

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName, strong;
	double Mass, CoGIxx, CoGIyy, CoGIzz, CoGIxz;
	CVector CoG;

	if(m_pPlane)     FileName = m_pPlane->m_PlaneName;
	else if(m_pWing) FileName = m_pWing->m_WingName;
	else if(m_pBody) FileName = m_pBody->m_BodyName;
	FileName.replace("/", " ");
	FileName += ".mass";
	FileName = QFileDialog::getSaveFileName(this, tr("Export Mass Properties"),pMainFrame->m_LastDirName + "/"+FileName,
	                                        tr("AVL Mass File (*.mass)"), &filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	pos = FileName.lastIndexOf(".");
	if(pos<0) FileName += ".mass";

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;
	QTextStream out(&XFile);
	out.setCodec("UTF-8");

	double Lunit = 1./pMainFrame->m_mtoUnit;
	double Munit = 1./pMainFrame->m_kgtoUnit;
	double Iunit = Munit * Lunit * Lunit;

	out << "#-------------------------------------------------\n";
	out << "#\n";
	if(m_pPlane)      out << "#   "+m_pPlane->m_PlaneName+"\n";
	else if(m_pWing)  out << "#   "+m_pWing->m_WingName+"\n";
	else if(m_pBody)  out << "#   "+m_pBody->m_BodyName+"\n";
	out << "#\n";
	out << "#  Dimensional unit and parameter data.\n";
	out << "#  Mass & Inertia breakdown.\n";
	out << "#-------------------------------------------------\n";
	out << "#  Names and scalings for units to be used for trim and eigenmode calculations.\n";
	out << "#  The Lunit and Munit values scale the mass, xyz, and inertia table data below.\n";
	out << "#  Lunit value will also scale all lengths and areas in the AVL input file.\n";
	out << "#\n";
	strong = QString("Lunit = %1 m").arg(Lunit,10,'f',4);
	out << strong+"\n";
	strong = QString("Munit = %1 kg").arg(Munit,10,'f',4);
	out << strong+"\n";
	out << "Tunit = 1.0 s\n";
	out << "#-------------------------\n";
	out << "#  Gravity and density to be used as default values in trim setup (saves runtime typing).\n";
	out << "#  Must be in the unit names given above (i.e. m,kg,s).\n";
	out << "g   = 9.81\n";
	out << "rho = 1.225\n";
	out << "#-------------------------\n";
	out << "#  Mass & Inertia breakdown.\n";
	out << "#  x y z  is location of item's own CG.\n";
	out << "#  Ixx... are item's inertias about item's own CG.\n";
	out << "#\n";
	out << "#  x,y,z system here must be exactly the same one used in the .avl input file\n";
	out << "#     (same orientation, same origin location, same length units)\n";
	out << "#\n";
	out << "#     mass          x          y          z        Ixx        Iyy        Izz      [  Ixy   Ixz   Iyz ]\n";

	if(m_pWing)
	{
		// in accordance with AVL input format,
		// we need to convert the inertia in the wing's CoG system
		// by applying Huyghen/Steiner's theorem

		strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Inertia of both left and right wings"))
						.arg(m_Mass /Munit,  10, 'g', 3)
						.arg(m_VolumeCoG.x/Lunit, 10, 'g', 3)
						.arg(m_VolumeCoG.y/Lunit, 10, 'g', 3)  //should be zero
						.arg(m_VolumeCoG.z/Lunit, 10, 'g', 3)
						.arg(m_CoGIxx/Iunit,  10, 'g', 3).arg(m_CoGIyy/Iunit,  10, 'g', 3).arg(m_CoGIzz/Iunit,  10, 'g', 3);
		out << strong+"\n";
	}
	else if (m_pBody)
	{
		strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Body inertia"))
						.arg(m_Mass /Munit, 10, 'g', 3)
						.arg(m_VolumeCoG.x/Lunit, 10, 'g', 3)
						.arg(m_VolumeCoG.y/Lunit, 10, 'g', 3)
						.arg(m_VolumeCoG.z/Lunit, 10, 'g', 3)
						.arg(m_CoGIxx/Iunit,  10, 'g', 3).arg(m_CoGIyy/Iunit,  10, 'g', 3).arg(m_CoGIzz/Iunit,  10, 'g', 3);
		out << strong+"\n";
	}
	else if (m_pPlane)
	{
		// we write out each object contribution individually
		// a plane has always a wing
		m_pPlane->m_Wing.ComputeVolumeInertia(m_pPlane->m_Wing.m_Mass, CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
		strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Main wing's inertia"))
						.arg(m_pPlane->m_Wing.m_Mass /Munit, 10, 'g', 3)
						.arg(CoG.x/Lunit, 10, 'g', 3)
						.arg(CoG.y/Lunit, 10, 'g', 3)
						.arg(CoG.z/Lunit, 10, 'g', 3)
						.arg(CoGIxx/Iunit,10, 'g', 3)
						.arg(CoGIyy/Iunit,10, 'g', 3)
						.arg(CoGIzz/Iunit,10, 'g', 3);
		out << strong+"\n";

		if(m_pPlane->m_bBiplane)
		{
			m_pPlane->m_Wing2.ComputeVolumeInertia(m_pPlane->m_Wing2.m_Mass, CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
			strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Second wing's inertia"))
						 .arg(m_pPlane->m_Wing2.m_Mass /Munit, 10, 'g', 3)
						 .arg(CoG.x/Lunit, 10, 'g', 3)
						 .arg(CoG.y/Lunit, 10, 'g', 3)
						 .arg(CoG.z/Lunit, 10, 'g', 3)
						 .arg(CoGIxx/Iunit,10, 'g', 3)
						 .arg(CoGIyy/Iunit,10, 'g', 3)
						 .arg(CoGIzz/Iunit,10, 'g', 3);
			out << strong+"\n";
		}
		if(m_pPlane->m_bStab)
		{
			m_pPlane->m_Stab.ComputeVolumeInertia(m_pPlane->m_Stab.m_Mass, CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
			strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Elevator's inertia"))
						 .arg(m_pPlane->m_Stab.m_Mass /Munit, 10, 'g', 3)
						 .arg(CoG.x/Lunit, 10, 'g', 3)
						 .arg(CoG.y/Lunit, 10, 'g', 3)
						 .arg(CoG.z/Lunit, 10, 'g', 3)
						 .arg(CoGIxx/Iunit,10, 'g', 3)
						 .arg(CoGIyy/Iunit,10, 'g', 3)
						 .arg(CoGIzz/Iunit,10, 'g', 3);
			out << strong+"\n";
		}
		if(m_pPlane->m_bFin)
		{
			m_pPlane->m_Fin.ComputeVolumeInertia(m_pPlane->m_Fin.m_Mass, CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
			strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Fin's inertia"))
						 .arg(m_pPlane->m_Fin.m_Mass /Munit, 10, 'g', 3)
						 .arg(CoG.x/Lunit, 10, 'g', 3)
						 .arg(CoG.y/Lunit, 10, 'g', 3)
						 .arg(CoG.z/Lunit, 10, 'g', 3)
						 .arg(CoGIxx/Iunit,10, 'g', 3)
						 .arg(CoGIyy/Iunit,10, 'g', 3)
						 .arg(CoGIzz/Iunit,10, 'g', 3);
			out << strong+"\n";
		}
		if(m_pPlane->m_bBody)
		{
			m_pPlane->m_pBody->ComputeBodyInertia(m_pPlane->m_pBody->m_Mass, CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
			strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 ! Body's inertia"))
						 .arg(m_pPlane->m_pBody->m_Mass /Munit, 10, 'g', 3)
						 .arg(CoG.x/Lunit, 10, 'g', 3)
						 .arg(CoG.y/Lunit, 10, 'g', 3)
						 .arg(CoG.z/Lunit, 10, 'g', 3)
						 .arg(CoGIxx/Iunit,10, 'g', 3)
						 .arg(CoGIyy/Iunit,10, 'g', 3)
						 .arg(CoGIzz/Iunit,10, 'g', 3);
			out << strong+"\n";
		}
	}

	for (int i=0; i<MAXMASSES; i++)
	{
		if(m_MassValue[i]>0.0)
		{
			strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
				.arg(m_MassValue[i] / Munit,    10, 'g', 3)
				.arg(m_MassPosition[i].x/Lunit, 10, 'g', 3)
				.arg(m_MassPosition[i].y/Lunit, 10, 'g', 3)
				.arg(m_MassPosition[i].z/Lunit, 10, 'g', 3);
			strong += " ! " + m_MassTag[i];
			out << strong+"\n";
		}
	}

	if(m_pPlane)
	{
		// need to write the point masses for the objects
		//Main Wing
		for (int i=0; i<m_pPlane->m_Wing.m_NMass; i++)
		{
			if(m_pPlane->m_Wing.m_MassValue[i]>0.0)
			{
				strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
					.arg(m_pPlane->m_Wing.m_MassValue[i] / Munit,    10, 'g', 3)
					.arg(m_pPlane->m_Wing.m_MassPosition[i].x/Lunit, 10, 'g', 3)
					.arg(m_pPlane->m_Wing.m_MassPosition[i].y/Lunit, 10, 'g', 3)
					.arg(m_pPlane->m_Wing.m_MassPosition[i].z/Lunit, 10, 'g', 3);
				strong += " ! " + m_pPlane->m_Wing.m_MassTag[i];
				out << strong+"\n";
			}
		}

		if(m_pPlane->m_bBiplane)
		{
			//Second Wing
			for (int i=0; i<m_pPlane->m_Wing2.m_NMass; i++)
			{
				if(m_pPlane->m_Wing.m_MassValue[i]>0.0)
				{
					strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
						.arg(m_pPlane->m_Wing2.m_MassValue[i] / Munit,    10, 'g', 3)
						.arg(m_pPlane->m_Wing2.m_MassPosition[i].x/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_Wing2.m_MassPosition[i].y/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_Wing2.m_MassPosition[i].z/Lunit, 10, 'g', 3);
					strong += " ! " + m_pPlane->m_Wing2.m_MassTag[i];
					out << strong+"\n";
				}
			}
		}

		if(m_pPlane->m_bStab)
		{
			//Elevator
			for (int i=0; i<m_pPlane->m_Stab.m_NMass; i++)
			{
				if(m_pPlane->m_Wing.m_MassValue[i]>0.0)
				{
					strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
						.arg(m_pPlane->m_Stab.m_MassValue[i] / Munit,    10, 'g', 3)
						.arg(m_pPlane->m_Stab.m_MassPosition[i].x/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_Stab.m_MassPosition[i].y/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_Stab.m_MassPosition[i].z/Lunit, 10, 'g', 3);
					strong += " ! " + m_pPlane->m_Stab.m_MassTag[i];
					out << strong+"\n";
				}
			}
		}

		if(m_pPlane->m_bFin)
		{
			//fin
			for (int i=0; i<m_pPlane->m_Fin.m_NMass; i++)
			{
				if(m_pPlane->m_Fin.m_MassValue[i]>0.0)
				{
					strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
						.arg(m_pPlane->m_Fin.m_MassValue[i] / Munit,    10, 'g', 3)
						.arg(m_pPlane->m_Fin.m_MassPosition[i].x/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_Fin.m_MassPosition[i].y/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_Fin.m_MassPosition[i].z/Lunit, 10, 'g', 3);
					strong += " ! " + m_pPlane->m_Fin.m_MassTag[i];
					out << strong+"\n";
				}
			}
		}
		if(m_pPlane->m_bBody)
		{
			//fin
			for (int i=0; i<m_pPlane->m_Fin.m_NMass; i++)
			{
				if(m_pPlane->m_Fin.m_MassValue[i]>0.0)
				{
					strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
						.arg(m_pPlane->m_pBody->m_MassValue[i] / Munit,    10, 'g', 3)
						.arg(m_pPlane->m_pBody->m_MassPosition[i].x/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_pBody->m_MassPosition[i].y/Lunit, 10, 'g', 3)
						.arg(m_pPlane->m_pBody->m_MassPosition[i].z/Lunit, 10, 'g', 3);
					strong += " ! " + m_pPlane->m_pBody->m_MassTag[i];
					out << strong+"\n";
				}
			}
		}
	}

	XFile.close();
}



void InertiaDlg::OnOK()
{
	int i,j;
	ReadData();

	if(m_pWing)
	{
		j=0;
		m_pWing->m_Mass = m_Mass;
		for(i=0; i< MAXMASSES; i++)
		{
			if(m_MassValue[i]>1.0e-30)
			{
				m_pWing->m_MassValue[j] = m_MassValue[i];
				m_pWing->m_MassPosition[j].Copy(m_MassPosition[i]);
				m_pWing->m_MassTag[j]   = m_MassTag[i];
				j++;
			}			
		}
		m_pWing->m_NMass = j;
	}
	else if(m_pBody)
	{
		j=0;
		m_pBody->m_Mass = m_Mass;
		for(i=0; i< MAXMASSES; i++)
		{
			if(m_MassValue[i]>1.0e-30)
			{
				m_pBody->m_MassValue[j] = m_MassValue[i];
				m_pBody->m_MassPosition[j].Copy(m_MassPosition[i]);
				m_pBody->m_MassTag[j]   = m_MassTag[i];
				j++;
			}
		}
		m_pBody->m_NMass = j;
	}
	else if(m_pPlane)
	{
		j=0;
		for(i=0; i< MAXMASSES; i++)
		{
			if(m_MassValue[i]>1.0e-30)
			{
				m_pPlane->m_MassValue[j] = m_MassValue[i];
				m_pPlane->m_MassPosition[j].Copy(m_MassPosition[i]);
				m_pPlane->m_MassTag[j]   = m_MassTag[i];
				j++;
			}
		}
		m_pPlane->m_NMass = j;
	}
	accept();
}


void InertiaDlg::ReadData()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QModelIndex index ;
	bool bOK;
	double val;
	int i;
	for (i=0; i<MAXMASSES; i++)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK)
		{
			m_MassValue[i]=val/pMainFrame->m_kgtoUnit;
		}

		index = m_pMassModel->index(i, 1, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassPosition[i].x=val/pMainFrame->m_mtoUnit;

		index = m_pMassModel->index(i, 2, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassPosition[i].y=val/pMainFrame->m_mtoUnit;

		index = m_pMassModel->index(i, 3, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassPosition[i].z=val/pMainFrame->m_mtoUnit;

		index = m_pMassModel->index(i, 4, QModelIndex());
		m_MassTag[i] = index.data().toString();
	}
	m_NMass = i;

	m_Mass = m_pctrlVolumeMass->GetValue() / pMainFrame->m_kgtoUnit;
}



void InertiaDlg::resizeEvent(QResizeEvent *event)
{
	double w = (double)m_pctrlMassView->width()*.97;
	int w6  = (int)(w/6.);

	m_pctrlMassView->setColumnWidth(0, w6);
	m_pctrlMassView->setColumnWidth(1, w6);
	m_pctrlMassView->setColumnWidth(2, w6);
	m_pctrlMassView->setColumnWidth(3, w6);
	m_pctrlMassView->setColumnWidth(4, w-4*w6);
}


void InertiaDlg::SetupLayout()
{
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	QVBoxLayout *MessageLayout = new QVBoxLayout;
	QLabel *Label1 = new QLabel(tr("This is a calculation form for a rough order of magnitude for the inertia tensor."));
	QLabel *Label2 = new QLabel(tr("Refer to the Guidelines for explanations."));
	MessageLayout->addWidget(Label1);
	MessageLayout->addWidget(Label2);

	//___________Volume Mass, Center of gravity, and inertias__________
	QGroupBox *ObjectMassBox = new QGroupBox(tr("Object Mass - Volume only, excluding point masses"));
	QHBoxLayout *ObjectMassLayout = new QHBoxLayout;
	m_pctrlVolumeMassLabel  = new QLabel(tr("Wing Mass="));
	m_pctrlMassUnit   = new QLabel("kg");
	m_pctrlVolumeMass = new FloatEdit(1.00,3);

	QGridLayout *CoGLayout = new QGridLayout;
	QLabel *CoGLabel = new QLabel(tr("Center of gravity"));
	CoGLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *XCoGLab = new QLabel("X_CoG=");
	QLabel *YCoGLab = new QLabel("Y_CoG=");
	QLabel *ZCoGLab = new QLabel("Z_CoG=");
	m_pctrlXCoG = new FloatEdit(0.00,3);
	m_pctrlYCoG = new FloatEdit(0.00,3);
	m_pctrlZCoG = new FloatEdit(0.00,3);
	m_pctrlXCoG->setEnabled(false);
	m_pctrlYCoG->setEnabled(false);
	m_pctrlZCoG->setEnabled(false);
	m_pctrlLengthUnit = new QLabel("m");
	m_pctrlVolumeMassLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	XCoGLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	YCoGLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	ZCoGLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlXCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlYCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlZCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	CoGLayout->addWidget(m_pctrlVolumeMassLabel,1,1);
	CoGLayout->addWidget(m_pctrlVolumeMass,1,2);
	CoGLayout->addWidget(m_pctrlMassUnit,1,3);
	CoGLayout->addWidget(XCoGLab,2,1);
	CoGLayout->addWidget(YCoGLab,3,1);
	CoGLayout->addWidget(ZCoGLab,4,1);
//	CoGLayout->addWidget(CoGLabel,2,1);
	CoGLayout->addWidget(m_pctrlXCoG,2,2);
	CoGLayout->addWidget(m_pctrlYCoG,3,2);
	CoGLayout->addWidget(m_pctrlZCoG,4,2);
	CoGLayout->addWidget(m_pctrlLengthUnit,2,3);
	CoGLayout->setColumnStretch(1,1);
	CoGLayout->setColumnStretch(2,2);
	CoGLayout->setColumnStretch(3,1);
	QGroupBox *CoGBox = new QGroupBox(tr("Center of gravity"));
	CoGBox->setLayout(CoGLayout);

	QGroupBox *ResultsBox = new QGroupBox(tr("Inertia in CoG Frame"));
	QGridLayout *ResultsLayout = new QGridLayout;
	m_pctrlCoGIxx = new FloatEdit(1.0,2);
	m_pctrlCoGIyy = new FloatEdit(1.2,2);
	m_pctrlCoGIzz = new FloatEdit(-1.5,2);
	m_pctrlCoGIxz = new FloatEdit(4.2,2);
	m_pctrlCoGIxx->setEnabled(false);
	m_pctrlCoGIyy->setEnabled(false);
	m_pctrlCoGIzz->setEnabled(false);
	m_pctrlCoGIxz->setEnabled(false);
	QLabel *LabIxx = new QLabel("Ixx=");
	QLabel *LabIyy = new QLabel("Iyy=");
	QLabel *LabIzz = new QLabel("Izz=");
	QLabel *LabIxz = new QLabel("Ixz=");
	LabIxx->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabIyy->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabIzz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabIxz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *LabInertiaObject = new QLabel(tr("Inertia in CoG Frame"));
	LabInertiaObject->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlInertiaUnit1 = new QLabel("kg.m2");
	ResultsLayout->addWidget(LabIxx,1,1);
	ResultsLayout->addWidget(LabIyy,2,1);
	ResultsLayout->addWidget(LabIzz,3,1);
	ResultsLayout->addWidget(LabIxz,4,1);
	ResultsLayout->addWidget(m_pctrlCoGIxx,1,2);
	ResultsLayout->addWidget(m_pctrlCoGIyy,2,2);
	ResultsLayout->addWidget(m_pctrlCoGIzz,3,2);
	ResultsLayout->addWidget(m_pctrlCoGIxz,4,2);
//	ResultsLayout->addWidget(LabInertiaObject,2,1);
	ResultsLayout->addWidget(m_pctrlInertiaUnit1,1,3);
	ResultsLayout->setColumnStretch(1,1);
	ResultsLayout->setColumnStretch(2,2);
	ResultsLayout->setColumnStretch(3,1);
	ResultsBox->setLayout(ResultsLayout);

	ObjectMassLayout->addWidget(CoGBox);
	ObjectMassLayout->addWidget(ResultsBox);
	ObjectMassBox->setLayout(ObjectMassLayout);

	//___________________Point Masses__________________________
	QGroupBox *PointMassBox = new QGroupBox(tr("Additional Point Masses:"));
	QVBoxLayout *MassLayout = new QVBoxLayout;
	m_pctrlMassView = new QTableView(this);
	m_pctrlMassView->setMinimumHeight(150);
	m_pctrlMassView->setMinimumWidth(350);
	m_pctrlMassView->setSelectionBehavior(QAbstractItemView::SelectRows);
	MassLayout->addWidget(m_pctrlMassView);
	PointMassBox->setLayout(MassLayout);

	//________________Total Mass, Center of gravity, and inertias__________
	QGroupBox *TotalMassBox = new QGroupBox(tr("Total Mass = Volume + point masses"));
	QHBoxLayout *TotalMassLayout = new QHBoxLayout;
	m_pctrlTotalMassLabel   = new QLabel(tr("Total Mass="));
	m_pctrlMassUnit2        = new QLabel("kg");
	m_pctrlTotalMass        = new FloatEdit(1.00,3);
	m_pctrlTotalMass->setEnabled(false);

	QGridLayout *TotalCoGLayout = new QGridLayout;
	QLabel *TotalLabel = new QLabel(tr("Center of gravity"));
	TotalLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *XTotalLab = new QLabel("X_CoG=");
	QLabel *YTotalLab = new QLabel("Y_CoG=");
	QLabel *ZTotalLab = new QLabel("Z_CoG=");
	m_pctrlXTotalCoG = new FloatEdit(0.00,3);
	m_pctrlYTotalCoG = new FloatEdit(0.00,3);
	m_pctrlZTotalCoG = new FloatEdit(0.00,3);
	m_pctrlXTotalCoG->setEnabled(false);
	m_pctrlYTotalCoG->setEnabled(false);
	m_pctrlZTotalCoG->setEnabled(false);
	m_pctrlLengthUnit2 = new QLabel("m");
	m_pctrlTotalMassLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	XTotalLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	YTotalLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	ZTotalLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlXTotalCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlYTotalCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlZTotalCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	TotalCoGLayout->addWidget(m_pctrlTotalMassLabel,1,1);
	TotalCoGLayout->addWidget(m_pctrlTotalMass,1,2);
	TotalCoGLayout->addWidget(m_pctrlMassUnit2,1,3);
	TotalCoGLayout->addWidget(XTotalLab,2,1);
	TotalCoGLayout->addWidget(YTotalLab,3,1);
	TotalCoGLayout->addWidget(ZTotalLab,4,1);
//	TotalCoGLayout->addWidget(TotalLabel,2,1);
	TotalCoGLayout->addWidget(m_pctrlXTotalCoG,2,2);
	TotalCoGLayout->addWidget(m_pctrlYTotalCoG,3,2);
	TotalCoGLayout->addWidget(m_pctrlZTotalCoG,4,2);
	TotalCoGLayout->addWidget(m_pctrlLengthUnit2,2,3);
	TotalCoGLayout->setColumnStretch(1,1);
	TotalCoGLayout->setColumnStretch(2,2);
	TotalCoGLayout->setColumnStretch(3,1);
	QGroupBox *TotalCoGBox = new QGroupBox(tr("Center of gravity"));
	TotalCoGBox->setLayout(TotalCoGLayout);

	QGroupBox *TotalInertiaBox = new QGroupBox(tr("Inertia in CoG Frame"));
	QGridLayout *TotalInertiaLayout = new QGridLayout;
	m_pctrlTotalIxx = new FloatEdit(1.0,2);
	m_pctrlTotalIyy = new FloatEdit(1.2,2);
	m_pctrlTotalIzz = new FloatEdit(-1.5,2);
	m_pctrlTotalIxz = new FloatEdit(4.2,2);
	m_pctrlTotalIxx->setEnabled(false);
	m_pctrlTotalIyy->setEnabled(false);
	m_pctrlTotalIzz->setEnabled(false);
	m_pctrlTotalIxz->setEnabled(false);
	QLabel *LabTotIxx = new QLabel("Ixx=");
	QLabel *LabTotIyy = new QLabel("Iyy=");
	QLabel *LabTotIzz = new QLabel("Izz=");
	QLabel *LabTotIxz = new QLabel("Ixz=");
	LabTotIxx->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabTotIyy->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabTotIzz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabTotIxz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *LabInertiaTotal = new QLabel(tr("Inertia in CoG Frame"));
	LabInertiaTotal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlInertiaUnit2 = new QLabel("kg.m2");
	TotalInertiaLayout->addWidget(LabTotIxx,1,1);
	TotalInertiaLayout->addWidget(LabTotIyy,2,1);
	TotalInertiaLayout->addWidget(LabTotIzz,3,1);
	TotalInertiaLayout->addWidget(LabTotIxz,4,1);
	TotalInertiaLayout->addWidget(m_pctrlTotalIxx,1,2);
	TotalInertiaLayout->addWidget(m_pctrlTotalIyy,2,2);
	TotalInertiaLayout->addWidget(m_pctrlTotalIzz,3,2);
	TotalInertiaLayout->addWidget(m_pctrlTotalIxz,4,2);
//	TotalInertiaLayout->addWidget(LabInertiaObject,2,1);
	TotalInertiaLayout->addWidget(m_pctrlInertiaUnit2,1,3);
	TotalInertiaLayout->setColumnStretch(1,1);
	TotalInertiaLayout->setColumnStretch(2,2);
	TotalInertiaLayout->setColumnStretch(3,1);
	TotalInertiaBox->setLayout(TotalInertiaLayout);

	TotalMassLayout->addWidget(TotalCoGBox);
	TotalMassLayout->addWidget(TotalInertiaBox);
	TotalMassBox->setLayout(TotalMassLayout);

	//__________________Control buttons___________________

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *ExportAVLButton = new QPushButton(tr("Export to AVL"));
	OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ExportAVLButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	MainLayout->addStretch(1);
	MainLayout->addLayout(MessageLayout);
	MainLayout->addStretch(1);
	MainLayout->addWidget(ObjectMassBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(PointMassBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(TotalMassBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ExportAVLButton, SIGNAL(clicked()), this, SLOT(OnExportToAVL()));
	connect(m_pctrlVolumeMass, SIGNAL(editingFinished()), SLOT(OnCellChanged()));
}


void InertiaDlg::showEvent(QShowEvent *event)
{
	resizeEvent(NULL);
}