#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <nlohmann/json.hpp>

#include <QLabel>

#include "ChessRenderImage.hpp"

class RenderDialog : public QDialog {
	public:
		RenderDialog(RenderImage &img, nlohmann::json &work, QWidget *p_Parent = nullptr);
	private:
		QPointer<QGridLayout> m_gridLayout;
		QPointer<QSpinBox> m_startIndex;
		QPointer<QSpinBox> m_endIndex;
		QPointer<QPushButton> m_renderVideo;
		//QPointer<QProgressBar> m_bar;
		//QPointer<QCheckBox> m_videoCheck;

		QPointer<QLineEdit> m_nameFile;

		/// Element

		RenderImage &m_img;
		nlohmann::json &m_work;
};
