#pragma once

#include <QApplication>
#include <QShortcut>
#include <future>
#include <QResizeEvent>
#include <QSpinBox>
#include <QFormLayout>
#include <QWidget>
#include <QPushButton>
#include <QPointer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QObject>
#include <QTextEdit>
#include <QKeyEvent>
#include "IDECode.hpp"

#include <nlohmann/json.hpp>

#include <QImage>
#include <vector>

#include <opencv2/opencv.hpp>

#include "SyntaxJson.hpp"
#include "ChessRenderImage.hpp"

#include <QThread>

#define IMAGE_FPS 24

#define SIZE_GRID_LAYOUT 500
#define IDE_MIN_SPACE 300

/// @brief the goal of this class is to make an IDE
/// @author Thibaud VINCENT
class ChessWindow : public QWidget {
	public:
		/// @brief The constructer for the graphics interface
		ChessWindow(const std::string &jfile);

		/// @brief the descrutor of the class
		~ChessWindow();

		/// @brief to get the move name
		/// @return QString (0start 1middle 3end) 
		QString getMoveMoment();

		/// @brief auto intent json file
		/// @return QString
		QString autoIntentJsonData(const QString &txt);

		/// @brief render the json to video (cv::Mat)
		/// @param indexStart the move start
		/// @param indexEnd the move end
		/// @param video if yes or no we render the video
		/// @return void
		void renderVideoJson();

		/// @brief to save the key
		void saveJson(std::string_view str_j);

		/// @brief to save all settings
		void saveAllSettings();

		/// @brief to render the js code on the screen
		/// @return void
		void testIt();

		/// @brief checker json
		bool checkJson(nlohmann::json &j);

		/// @brief to set the text inside the ide
		/// @return void
		void setTextValue();

		/// @brief to go to the next move
		/// @return void
		void nextMove();

		/// @brief to go to the penultimate move
		/// @return void
		void backMove();

		/// @brief to test it json code works
		/// @return void
		bool compileIt();

		/// @brief when the text change (slot disable)
		/// @return void
		void textChange();

		/// @brief To add text inside the IDE
		/// @param txt the text to add
		/// @return void
		void addText(QString txt);

		/// @brief to render a new image
		/// @return void
		void setupNewImage();
	
		void pause();
		/// @brief The qthread to make the video possible
		/// @return void
		static void videoMaker(std::vector<cv::Mat>* s_image, short *continueAlive, QPointer<QLabel> *m_imgDisplayLabel);
	protected:
		void resizeEvent(QResizeEvent *event) override;
		void keyPressEvent(QKeyEvent *event) override;

	private:
		static std::mutex s_m;
		static int s_speed;
		static QSize s_window_size;
		static size_t frameRateCurrent;

		/// The layouts

		QPointer<QHBoxLayout> m_chessMainLayout;
		QPointer<QVBoxLayout> m_chessJsonLayout;

		QPointer<QVBoxLayout> m_chessRight;
		QPointer<QGridLayout> m_chessFormSettings;

		/// The image part

		QPointer<QLabel> m_imgDisplayLabel;
		std::vector<cv::Mat> m_image;
		short continueAlive;
		std::future<void> m_function;
		std::future<void> m_resize;

		/// The json ide

		QPointer<QLabel> m_textJson;
		QPointer<CodeEditor> m_jsonValue;
		QPointer<Highlighter> highlighter;
		QPointer<QLabel> m_jsonError;

		/// JsonCommand

		std::size_t current;
		int type;

		RenderImage img;

		/// The button
		
		QPointer<QPushButton> m_testJson;
		QPointer<QPushButton> m_rigthJson;
		QPointer<QPushButton> m_leftJson;
		QPointer<QPushButton> m_render;

		/// The settings

		QPointer<QSpinBox> m_frameRate;
		QPointer<QSpinBox> m_defaultTime;
		QPointer<QSpinBox> m_speed;
		QPointer<QPushButton> m_apply;

		QPointer<QPushButton> m_demo;
		QPointer<QPushButton> m_save;
		QPointer<QPushButton> m_pause;

		QPointer<QShortcut> m_ctrlS;

		nlohmann::json allSettings;

		const std::string &fileSettings;
};
