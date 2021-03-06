/****************************************************************************

	CLEDlg class
	Copyright (C) 2007 Andr� Deperrois xflr5@yahoo.com

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

#pragma once

// LEDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "XFoil.h"
#include "Foil.h"

/////////////////////////////////////////////////////////////////////////////
// CLEDlg dialog

class CLEDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;
// Construction
public:
	CLEDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLEDlg)
	enum { IDD = IDD_LEDLG };
	CButton	m_ctrlOK;
	CFloatEdit	m_ctrlLE;
	CFloatEdit	m_ctrlBlend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLEDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bApplied, m_bModified;
	double m_LErfac, m_Blend;

//	CWnd *m_pParent;
	CWnd *m_pChildView;

	CFoil* m_pBufferFoil;
	CFoil* m_pMemFoil;
	XFoil* m_pXFoil;


	// Generated message map functions
	//{{AFX_MSG(CLEDlg)
	virtual void OnOK();
	afx_msg void OnApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

