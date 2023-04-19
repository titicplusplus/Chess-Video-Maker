#include "ChessDialog.hpp"
#include <QLabel>
#include <chrono>

RenderDialog::RenderDialog(RenderImage &img, nlohmann::json &work, QWidget *p_Parent) : m_img {img}, m_work {work} {
	setWindowIcon(QIcon("chess.ico"));

	resize(500, 300);

	m_gridLayout = new QGridLayout(this);
	m_gridLayout->setContentsMargins(15, 15, 15, 15);
	m_gridLayout->setSpacing(15);
	m_gridLayout->setMargin(9);

	m_startIndex = new QSpinBox();
	m_endIndex = new QSpinBox();

	if (img.sizeCmoveIDE() == 0) {
		m_startIndex->setMaximum(0);
		m_endIndex->setMaximum(0);
	} else {
		m_startIndex->setMaximum(img.sizeCmoveIDE() - 1);
		m_endIndex->setMaximum(img.sizeCmoveIDE() - 1);
		m_endIndex->setValue(img.sizeCmoveIDE() - 1);
	}

	m_renderVideo = new QPushButton("Render the video");
	m_renderVideo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QObject::connect(m_renderVideo, &QPushButton::clicked, [this, work] {
		std::vector<std::vector<cv::Mat>> renderVec;
		m_img.fastRenderAsync(m_startIndex->value(), m_endIndex->value(), m_work, renderVec, 
				m_nameFile->text().toStdString());
	});

	m_nameFile = new QLineEdit("outcpp.avi");

	m_gridLayout->addWidget(new QLabel("Index start:"), 0, 0, 1, 1);
	m_gridLayout->addWidget(new QLabel("Index end:"), 1, 0, 1, 1);

	m_gridLayout->addWidget(m_startIndex, 0, 1, 1, 1);
	m_gridLayout->addWidget(m_endIndex, 1, 1, 1, 1);
	m_gridLayout->addWidget(m_nameFile, 2, 0, 1, 2);

	m_gridLayout->addWidget(m_renderVideo, 3, 0, 1, 2);

	exec();
}


