/****************************************************************************

	InertiaDlg Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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
		m_MassTag[i] = QString("Description %1").arg(i);
	}

	SetupLayout();
}


void InertiaDlg::ComputeInertia()
{
	//assumes that the data has been read
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_CoG.Set(0.0, 0.0, 0.0);
	m_PtRef.Set(0.0, 0.0, 0.0);
	int i;

	//First evaluate the object's inertia

	if(m_pWing)
	{
		m_pWing->ComputeInertia(m_Mass);
		m_CoG.Copy(m_pWing->m_CoG);
		m_CoGIxx = m_pWing->m_CoGIxx;
		m_CoGIyy = m_pWing->m_CoGIyy;
		m_CoGIzz = m_pWing->m_CoGIzz;
		m_CoGIxz = m_pWing->m_CoGIxz;
	}
	else if(m_pBody)
	{
		m_pBody->ComputeBodyInertia(m_Mass);
		m_CoG.Copy(m_pBody->m_CoG);
		m_CoGIxx = m_pBody->m_CoGIxx;
		m_CoGIyy = m_pBody->m_CoGIyy;
		m_CoGIzz = m_pBody->m_CoGIzz;
		m_CoGIxz = m_pBody->m_CoGIxz;
	}
	else if(m_pPlane)
	{
		m_pPlane->ComputeInertia(m_Mass);
		m_CoG.Copy(m_pPlane->m_CoG);
		m_CoGIxx = m_pPlane->m_CoGIxx;
		m_CoGIyy = m_pPlane->m_CoGIyy;
		m_CoGIzz = m_pPlane->m_CoGIzz;
		m_CoGIxz = m_pPlane->m_CoGIxz;
	}


	//and display the results
	double Unit = pMainFrame->m_mtoUnit * pMainFrame->m_mtoUnit * pMainFrame->m_kgtoUnit;

	m_pctrlXCoG->SetValue(m_CoG.x*pMainFrame->m_mtoUnit);
	m_pctrlYCoG->SetValue(m_CoG.y*pMainFrame->m_mtoUnit);
	m_pctrlZCoG->SetValue(m_CoG.z*pMainFrame->m_mtoUnit);

	m_pctrlCoGIxx->SetValue(m_CoGIxx*Unit);
	m_pctrlCoGIyy->SetValue(m_CoGIyy*Unit);
	m_pctrlCoGIzz->SetValue(m_CoGIzz*Unit);
	m_pctrlCoGIxz->SetValue(m_CoGIxz*Unit);
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
	GetLengthUnit(str1, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit->setText(str1);
	m_pctrlLengthUnit2->setText(str1);
	strong = str+"."+str1+"2";
	m_pctrlInertiaUnit1->setText(strong);
//	m_pctrlInertiaUnit2->setText(strong);

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
		m_pctrlObjectMass->SetValue(m_pWing->m_Mass * pMainFrame->m_kgtoUnit); //we only display half a wing, AVL way
		m_pctrlObjectMassLabel->setText("Wing Mass:");
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
		m_pctrlObjectMass->SetValue(m_pBody->m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlObjectMassLabel->setText("Body Mass:");
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

		m_pctrlObjectMass->SetValue(m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlObjectMassLabel->setText("Plane Mass:");
		m_pctrlObjectMass->setEnabled(false);
	}

//	m_pctrlAuto->setChecked(true);
//	OnInputType();

	FillMassModel();
	ComputeInertia();
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
}


void InertiaDlg::OnExportToAVL()
{
	if (!m_pWing && !m_pBody && !m_pPlane) return;
	QString filter =".mass";

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName, DestFileName, OutString, strong;
	QFile DestFile;

	if(m_pPlane)     FileName = m_pPlane->m_PlaneName;
	else if(m_pWing) FileName = m_pWing->m_WingName;
	else if(m_pBody) FileName = m_pBody->m_BodyName;
	FileName.replace("/", " ");
	FileName += ".mass";
	FileName = QFileDialog::getSaveFileName(this, "Export Mass Properties",pMainFrame->m_LastDirName + "/"+FileName,
	                                        "AVL Mass File (*.mass)", &filter);
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
	out << "#   mass       x         y         z        Ixx       Iyy       Izz      [  Ixy   Ixz   Iyz ]\n";

	if(m_pWing)
	{
		// in accordance with AVL input format,
		// we need to convert the inertia in the wing's CoG system
		// by applying Huyghen/Steiner's theorem

		strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Inertia of both left and right wings")
						.arg(m_Mass /Munit,  8, 'f', 2)
						.arg(m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_CoG.y/Lunit, 8, 'f', 2)  //should be zero
						.arg(m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_CoGIxx/Iunit,  8, 'e', 3).arg(m_CoGIyy/Iunit,  8, 'e', 3).arg(m_CoGIzz/Iunit,  8, 'e', 3);
		out << strong+"\n";
	}
	else if (m_pBody)
	{
		strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Body inertia")
						.arg(m_Mass /Munit, 8, 'f', 2)
						.arg(m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_CoG.y/Lunit, 8, 'f', 2)
						.arg(m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_CoGIxx/Iunit,  8, 'e', 3).arg(m_CoGIyy/Iunit,  8, 'e', 3).arg(m_CoGIzz/Iunit,  8, 'e', 3);
		out << strong+"\n";
	}
	else if (m_pPlane)
	{
		// we write out each object contribution individually
		// a plane has always a wing
		strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Main wing's inertia")
						.arg(m_pPlane->m_Wing.m_Mass /Munit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing.m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing.m_CoG.y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing.m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing.m_CoGIxx/Iunit,8, 'e', 3)
						.arg(m_pPlane->m_Wing.m_CoGIyy/Iunit,8, 'e', 3)
						.arg(m_pPlane->m_Wing.m_CoGIzz/Iunit,8, 'e', 3);
		out << strong+"\n";

		if(m_pPlane->m_bBiplane)
		{
			strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Second wing's inertia")
						.arg(m_pPlane->m_Wing2.m_Mass /Munit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_CoG.y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_CoGIxx/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_Wing2.m_CoGIyy/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_Wing2.m_CoGIzz/Iunit,  8, 'e', 3);
			out << strong+"\n";
		}
		if(m_pPlane->m_bStab)
		{
			strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Elevator's inertia")
						.arg(m_pPlane->m_Stab.m_Mass /Munit, 8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_CoG.y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_CoGIxx/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_Stab.m_CoGIyy/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_Stab.m_CoGIzz/Iunit,  8, 'e', 3);
			out << strong+"\n";
		}
		if(m_pPlane->m_bFin)
		{
			strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Fin's inertia")
						.arg(m_pPlane->m_Fin.m_Mass /Munit, 8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_CoG.y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_CoGIxx/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_Fin.m_CoGIyy/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_Fin.m_CoGIzz/Iunit,  8, 'e', 3);
			out << strong+"\n";
		}
		if(m_pPlane->m_bBody)
		{
			strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Body's inertia")
						.arg(m_pPlane->m_pBody->m_Mass /Munit, 8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_CoG.x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_CoG.y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_CoG.z/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_CoGIxx/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_pBody->m_CoGIyy/Iunit,  8, 'e', 3)
						.arg(m_pPlane->m_pBody->m_CoGIzz/Iunit,  8, 'e', 3);
			out << strong+"\n";
		}
	}

	for (int i=0; i<MAXMASSES; i++)
	{
		if(m_MassValue[i]>0.0)
		{
			strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
				.arg(m_MassValue[i] / Munit,    8, 'f', 2)
				.arg(m_MassPosition[i].x/Lunit, 8, 'f', 2)
				.arg(m_MassPosition[i].y/Lunit, 8, 'f', 2)
				.arg(m_MassPosition[i].z/Lunit, 8, 'f', 2);
			strong += "    ! " + m_MassTag[i];
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
				strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
					.arg(m_pPlane->m_Wing.m_MassValue[i] / Munit,    8, 'f', 2)
					.arg(m_pPlane->m_Wing.m_MassPosition[i].x/Lunit, 8, 'f', 2)
					.arg(m_pPlane->m_Wing.m_MassPosition[i].y/Lunit, 8, 'f', 2)
					.arg(m_pPlane->m_Wing.m_MassPosition[i].z/Lunit, 8, 'f', 2);
				strong += "    ! " + m_pPlane->m_Wing.m_MassTag[i];
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
					strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
						.arg(m_pPlane->m_Wing2.m_MassValue[i] / Munit,    8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_MassPosition[i].x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_MassPosition[i].y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Wing2.m_MassPosition[i].z/Lunit, 8, 'f', 2);
					strong += "    ! " + m_pPlane->m_Wing2.m_MassTag[i];
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
					strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
						.arg(m_pPlane->m_Stab.m_MassValue[i] / Munit,    8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_MassPosition[i].x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_MassPosition[i].y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Stab.m_MassPosition[i].z/Lunit, 8, 'f', 2);
					strong += "    ! " + m_pPlane->m_Stab.m_MassTag[i];
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
					strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
						.arg(m_pPlane->m_Fin.m_MassValue[i] / Munit,    8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_MassPosition[i].x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_MassPosition[i].y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_Fin.m_MassPosition[i].z/Lunit, 8, 'f', 2);
					strong += "    ! " + m_pPlane->m_Fin.m_MassTag[i];
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
					strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
						.arg(m_pPlane->m_pBody->m_MassValue[i] / Munit,    8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_MassPosition[i].x/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_MassPosition[i].y/Lunit, 8, 'f', 2)
						.arg(m_pPlane->m_pBody->m_MassPosition[i].z/Lunit, 8, 'f', 2);
					strong += "    ! " + m_pPlane->m_pBody->m_MassTag[i];
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

	m_Mass = m_pctrlObjectMass->GetValue() / pMainFrame->m_kgtoUnit;
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
	QLabel *Label1 = new QLabel(tr("This is a calculation form for a rough order of magnitude for the inertia tensor. The results are used in stability calculations."));
	QLabel *Label2 = new QLabel(tr("Refer to the Guidelines for explanations."));
	MessageLayout->addWidget(Label1);
	MessageLayout->addWidget(Label2);

	QGroupBox *RefBox = new QGroupBox(tr("Reference Point"));
	m_pctrlXRef = new FloatEdit(0.0);
	m_pctrlYRef = new FloatEdit(0.0);
	m_pctrlZRef = new FloatEdit(0.0);
	QLabel *XLab = new QLabel("X");
	QLabel *YLab = new QLabel("Y");
	QLabel *ZLab = new QLabel("Z");
	XLab->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	YLab->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	ZLab->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	m_pctrlLengthUnit = new QLabel("m");
	QGridLayout *RefLayout = new QGridLayout;
	RefLayout->addWidget(XLab,1,1);
	RefLayout->addWidget(YLab,1,2);
	RefLayout->addWidget(ZLab,1,3);
	RefLayout->addWidget(m_pctrlXRef,2,1);
	RefLayout->addWidget(m_pctrlYRef,2,2);
	RefLayout->addWidget(m_pctrlZRef,2,3);
	RefLayout->addWidget(m_pctrlLengthUnit,2,4);
	RefBox->setLayout(RefLayout);


	QGroupBox *ObjectMassBox = new QGroupBox(tr("Object Mass"));
	QVBoxLayout *ObjectMassLayout = new QVBoxLayout;
	QHBoxLayout *WingMassLayout = new QHBoxLayout;
	m_pctrlObjectMassLabel  = new QLabel(tr("Wing Mass"));
	m_pctrlMassUnit = new QLabel("kg");
	m_pctrlObjectMass = new FloatEdit(1.00,3);
	WingMassLayout->addWidget(m_pctrlObjectMassLabel);
	WingMassLayout->addWidget(m_pctrlObjectMass);
	WingMassLayout->addWidget(m_pctrlMassUnit);
	WingMassLayout->addStretch(1);
	QGridLayout *CoGLayout = new QGridLayout;
	QLabel *CoGLabel = new QLabel(tr("Center of gravity"));
	CoGLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	QLabel *XCoGLab = new QLabel("X=");
	QLabel *YCoGLab = new QLabel("Y=");
	QLabel *ZCoGLab = new QLabel("Z=");
	m_pctrlXCoG = new FloatEdit(0.00,3);
	m_pctrlYCoG = new FloatEdit(0.00,3);
	m_pctrlZCoG = new FloatEdit(0.00,3);
	m_pctrlXCoG->setEnabled(false);
	m_pctrlYCoG->setEnabled(false);
	m_pctrlZCoG->setEnabled(false);
	m_pctrlLengthUnit2 = new QLabel("m");
	XCoGLab->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	YCoGLab->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	ZCoGLab->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	m_pctrlXCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlYCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlZCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	CoGLayout->addWidget(XCoGLab,1,2);
	CoGLayout->addWidget(YCoGLab,1,3);
	CoGLayout->addWidget(ZCoGLab,1,4);
	CoGLayout->addWidget(CoGLabel,2,1);
	CoGLayout->addWidget(m_pctrlXCoG,2,2);
	CoGLayout->addWidget(m_pctrlYCoG,2,3);
	CoGLayout->addWidget(m_pctrlZCoG,2,4);
	CoGLayout->addWidget(m_pctrlLengthUnit2,2,5);
	CoGLayout->setColumnStretch(1,2);
	CoGLayout->setColumnStretch(2,1);
	CoGLayout->setColumnStretch(3,1);
	CoGLayout->setColumnStretch(4,1);
	CoGLayout->setColumnStretch(5,2);


//	QGroupBox *ResultsBox = new QGroupBox(tr("Inertias : Volume only, without point masses"));
	QGridLayout *ResultsLayout = new QGridLayout;
	m_pctrlCoGIxx = new FloatEdit(1.0,2);
	m_pctrlCoGIyy = new FloatEdit(1.2,2);
	m_pctrlCoGIzz = new FloatEdit(-1.5,2);
	m_pctrlCoGIxz = new FloatEdit(4.2,2);
	m_pctrlCoGIxx->setEnabled(false);
	m_pctrlCoGIyy->setEnabled(false);
	m_pctrlCoGIzz->setEnabled(false);
	m_pctrlCoGIxz->setEnabled(false);
	QLabel *LabIxx = new QLabel("Ixx");
	QLabel *LabIyy = new QLabel("Iyy");
	QLabel *LabIzz = new QLabel("Izz");
	QLabel *LabIxz = new QLabel("Ixz");
	LabIxx->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LabIyy->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LabIzz->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LabIxz->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	QLabel *LabInertiaObject = new QLabel("Inertia in CoG Frame");
	LabInertiaObject->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlInertiaUnit1 = new QLabel("kg.m2");
	ResultsLayout->addWidget(LabIxx,1,2);
	ResultsLayout->addWidget(LabIyy,1,3);
	ResultsLayout->addWidget(LabIzz,1,4);
	ResultsLayout->addWidget(LabIxz,1,5);
	ResultsLayout->addWidget(m_pctrlCoGIxx,2,2);
	ResultsLayout->addWidget(m_pctrlCoGIyy,2,3);
	ResultsLayout->addWidget(m_pctrlCoGIzz,2,4);
	ResultsLayout->addWidget(m_pctrlCoGIxz,2,5);
	ResultsLayout->addWidget(LabInertiaObject,2,1);
	ResultsLayout->addWidget(m_pctrlInertiaUnit1,2,6);
	ResultsLayout->setColumnStretch(1,2);
	ResultsLayout->setColumnStretch(2,1);
	ResultsLayout->setColumnStretch(3,1);
	ResultsLayout->setColumnStretch(4,1);
	ResultsLayout->setColumnStretch(5,1);
	ResultsLayout->setColumnStretch(6,2);
//	ResultsBox->setLayout(ResultsLayout);

	ObjectMassLayout->addLayout(WingMassLayout);
	ObjectMassLayout->addLayout(CoGLayout);
	ObjectMassLayout->addLayout(ResultsLayout);
	ObjectMassBox->setLayout(ObjectMassLayout);

	QGroupBox *PointMassBox = new QGroupBox(tr("Additional Point Masses:"));
	QVBoxLayout *MassLayout = new QVBoxLayout;
	m_pctrlMassView = new QTableView(this);
	m_pctrlMassView->setMinimumHeight(200);
	m_pctrlMassView->setMinimumWidth(350);
	m_pctrlMassView->setSelectionBehavior(QAbstractItemView::SelectRows);
	MassLayout->addWidget(m_pctrlMassView);
	PointMassBox->setLayout(MassLayout);



	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *ExportAVLButton = new QPushButton(tr("Export to AVL"));
	OKButton = new QPushButton(tr("Close"));
//	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ExportAVLButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
//	CommandButtons->addWidget(CancelButton);
//	CommandButtons->addStretch(1);

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
//	MainLayout->addWidget(FirstLine);
	//MainLayout->addWidget(SecondLine);
	//MainLayout->addWidget(ThirdLine);
	MainLayout->addStretch(1);
	MainLayout->addLayout(MessageLayout);
	MainLayout->addStretch(1);
//	MainLayout->addWidget(RefBox);
//	MainLayout->addStretch(1);
	MainLayout->addWidget(ObjectMassBox);
//	MainLayout->addWidget(ResultsBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(PointMassBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
//	connect(m_pctrlAuto, SIGNAL(clicked()), this , SLOT(OnInputType()));
//	connect(m_pctrlMan, SIGNAL(clicked()), this , SLOT(OnInputType()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
//	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ExportAVLButton, SIGNAL(clicked()), this, SLOT(OnExportToAVL()));
	connect(m_pctrlObjectMass, SIGNAL(editingFinished()), SLOT(OnCellChanged()));
}


void InertiaDlg::showEvent(QShowEvent *event)
{
	resizeEvent(NULL);
}