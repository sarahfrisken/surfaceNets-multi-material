//
// newModelDialog.cpp
//
// Dialog for specifying parameters for opening a raw image file of material
// labels. Voxel material labels can either be unsigned short or unsigned char. This
// implementation of SurfaceNets supports labels from 0 to 65534.
//

#include "openModelFileDialog.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFileDialog>

OpenModelFileDialog::OpenModelFileDialog(QWidget* parent) :
	m_filename("")
{
	setWindowTitle(tr("New"));
	m_title = new QLabel(tr("Open raw data file (1 byte/voxel)"));
	m_dialogLayout = new QVBoxLayout();
	setLayout(m_dialogLayout);
	m_dialogLayout->addWidget(m_title);

	// Set default values
	int arraySize[3] = { 64, 64, 64 };
	float voxelSize[3] = { 1, 1, 1 };

	// Get filename
	m_filenameGroup = new QGroupBox();
	m_filenameLayout = new QHBoxLayout();
	m_filenameLabel = new QLabel("File path: ");
	m_filenameLineEdit = new QLineEdit();
	m_filenameLineEdit->setPlaceholderText("Path to raw data file");
	m_filenameBrowseButton = new QPushButton("Browse");
	connect(m_filenameBrowseButton, SIGNAL(clicked()), this, SLOT(onBrowseButton()));
	m_filenameLayout->addWidget(m_filenameLabel);
	m_filenameLayout->addWidget(m_filenameLineEdit);
	m_filenameLayout->addWidget(m_filenameBrowseButton);
	m_filenameGroup->setLayout(m_filenameLayout);
	m_dialogLayout->addWidget(m_filenameGroup);

	// Get data type
	m_dataTypeGroup = new QGroupBox("Data type");
	m_dataTypeLayout = new QFormLayout();
	m_dataType = new QComboBox;
	m_dataType->addItem(" ");
	m_dataType->addItem("Unsigned Char");
	m_dataType->addItem("Unsigned Short");
	m_dataTypeLayout->addRow(tr("Data format:"), m_dataType);
	m_dataTypeGroup->setLayout(m_dataTypeLayout);
	m_dialogLayout->addWidget(m_dataTypeGroup);

	// Get dimensions
	m_dimensionsGroup = new QGroupBox("Dimensions");
	m_dimensionsLayout = new QFormLayout();
	m_xDimInput = new QSpinBox;
	m_yDimInput = new QSpinBox;
	m_zDimInput = new QSpinBox;
	m_xDimInput->setValue(arraySize[0]);
	m_yDimInput->setValue(arraySize[1]);
	m_zDimInput->setValue(arraySize[2]);
	m_xDimInput->setMaximum(256);
	m_yDimInput->setMaximum(256);
	m_zDimInput->setMaximum(256);
	m_dimensionsLayout->addRow(tr("X dimension:"), m_xDimInput);
	m_dimensionsLayout->addRow(tr("Y dimension:"), m_yDimInput);
	m_dimensionsLayout->addRow(tr("Z dimension:"), m_zDimInput);
	m_dimensionsGroup->setLayout(m_dimensionsLayout);
	m_dialogLayout->addWidget(m_dimensionsGroup);

	// Get voxel sizes
	m_sizeGroup = new QGroupBox("Voxel size");
	m_sizeLayout = new QFormLayout;
	m_xSizeInput = new QDoubleSpinBox;
	m_ySizeInput = new QDoubleSpinBox;
	m_zSizeInput = new QDoubleSpinBox;
	m_xSizeInput->setValue(voxelSize[0]);
	m_ySizeInput->setValue(voxelSize[1]);
	m_zSizeInput->setValue(voxelSize[2]);
	m_sizeLayout->addRow(tr("X dimension:"), m_xSizeInput);
	m_sizeLayout->addRow(tr("Y dimension:"), m_ySizeInput);
	m_sizeLayout->addRow(tr("Z dimension:"), m_zSizeInput);
	m_sizeGroup->setLayout(m_sizeLayout);
	m_dialogLayout->addWidget(m_sizeGroup);

	// Cancel/OK buttons
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_dialogLayout->addWidget(m_buttonBox);
	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString OpenModelFileDialog::getFilename()
{
	return(m_filename);
};
int OpenModelFileDialog::getBytesPerVoxel()
{
	int dataTypeIdx = m_dataType->currentIndex();
	switch (dataTypeIdx) {
	case 0: 
	default:
	{
		// Unrecognized type
		return(-1);
	}
	case 1:
	{
		// Unsigned char: return 1 byte
		return(1);
	}
	case 2: {
		// Unsigned short: return 2 bytes
		return(2);
	}
	}
};
void OpenModelFileDialog::getDimensions(int dims[3])
{
	dims[0] = m_xDimInput->value();
	dims[1] = m_yDimInput->value();
	dims[2] = m_zDimInput->value();
};
void OpenModelFileDialog::getVoxelSize(float size[3])
{
	size[0] = m_xSizeInput->value();
	size[1] = m_ySizeInput->value();
	size[2] = m_zSizeInput->value();
};

void OpenModelFileDialog::onBrowseButton()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	m_filename = QFileDialog::getOpenFileName(0, ("Select Raw File"), QDir::currentPath());
	m_filenameLineEdit->setText(m_filename);
}
