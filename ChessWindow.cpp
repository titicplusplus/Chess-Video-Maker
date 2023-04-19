#include "ChessWindow.hpp"
#include "ChessDialog.hpp"
#include <mutex>
#include <chrono>
#include <thread>
#include <QLineEdit>
#include <QCommonStyle>
#include <QString>
#include <QDirIterator>
#include <QPainter>
#include <iostream>
#include <QFont>
#include "default.hpp"
#include <SFML/Audio.hpp>
#include <fstream>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

using json = nlohmann::json;

std::mutex ChessWindow::s_m;
int ChessWindow::s_speed = 1;
size_t ChessWindow::frameRateCurrent = 24;
QSize ChessWindow::s_window_size;

ChessWindow::ChessWindow(const std::string &jfile) : fileSettings{jfile} {

	if (fs::exists(jfile)) {
		std::ifstream file {jfile};
		file >> allSettings;
		file.close();
	} else {
		GetPuzzle puzzle;

		allSettings["render"] = {{"defaultTime", 16},{"frameRate", 24}};
		allSettings["data"] = puzzle.getPuzzle();

		std::ofstream outJ {jfile};
		outJ << std::setw(4) << allSettings << std::endl;
	}

	frameRateCurrent = allSettings["render"]["frameRate"];

	continueAlive = 2;
	current = 0;
	type = 0;

	img.startIDE(allSettings["data"]);

	QFont f{"Arial", 13, QFont::Bold};

	setWindowTitle("CppChess");
	setWindowIcon(QIcon("chess.ico"));

	/// Key shortcut
	
	m_ctrlS = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);

	QObject::connect(m_ctrlS, &QShortcut::activated, this, &ChessWindow::saveAllSettings);


	/// Layout 
	
	m_chessJsonLayout = new QVBoxLayout();
	m_chessJsonLayout->setContentsMargins(15, 15, 15, 15);

	m_chessMainLayout = new QHBoxLayout(this);
	m_chessMainLayout->setAlignment(Qt::AlignCenter);
	m_chessMainLayout->setContentsMargins(15, 15, 15, 15);

	m_chessRight = new QVBoxLayout();
	m_chessRight->setContentsMargins(15, 15, 15, 15);

	m_chessFormSettings = new QGridLayout();
	m_chessFormSettings->setContentsMargins(15, 15, 15, 15);

	/// Image
	
	m_imgDisplayLabel = new QLabel("");

	/// Top
	
	m_textJson = new QLabel("The json command: " + getMoveMoment());
	m_textJson->setFont(f);

	m_chessJsonLayout->addWidget(m_textJson);

	/// Middle
	
	//m_jsonValue = new QTextEdit();
	m_jsonValue = new CodeEditor();
	m_jsonValue->setFont(QFont("Courrier", 14, QFont::Normal));
	m_jsonValue->setStyleSheet("QTextEdit { background-color: rgb(26, 30, 36);}");

	#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
		m_jsonValue->setTabStopDistance(150);
	#endif

	highlighter = new Highlighter(m_jsonValue->document());

	m_chessJsonLayout->addWidget(m_jsonValue);

	/// Error
	
	m_jsonError = new QLabel("");
	m_jsonError->setFont(f);
	m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
	m_chessJsonLayout->addWidget(m_jsonError);

	/// Bottom
	
	m_testJson = new QPushButton("Test it");
	m_chessJsonLayout->addWidget(m_testJson, Qt::AlignBottom);

	QObject::connect(m_testJson, &QPushButton::clicked, [this] {
		this->testIt();
	});
	
	m_leftJson = new QPushButton("Go back");
	m_chessJsonLayout->addWidget(m_leftJson, Qt::AlignBottom);

	QObject::connect(m_leftJson, &QPushButton::clicked, [this] {
		this->backMove();
	});

	m_rigthJson = new QPushButton("Next");
	m_chessJsonLayout->addWidget(m_rigthJson, Qt::AlignBottom);

	QObject::connect(m_rigthJson, &QPushButton::clicked, [this] {
		this->nextMove();
	});

	m_render = new QPushButton("Render");
	m_chessJsonLayout->addWidget(m_render, Qt::AlignBottom);

	/// The settings part
	
	m_frameRate = new QSpinBox();
	m_frameRate->setValue(allSettings["render"]["frameRate"]);

	QObject::connect(m_frameRate, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int i) {
			std::lock_guard lck {s_m};
			//continueAlive = 3;
			frameRateCurrent = i;
			this->allSettings["render"]["frameRate"] = i;
	});

	m_defaultTime = new QSpinBox();
	m_defaultTime->setValue(allSettings["render"]["defaultTime"]);

	QObject::connect(m_defaultTime, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int i) {
			this->allSettings["render"]["defaultTime"] = i;
	});

	m_speed = new QSpinBox();
	m_speed->setValue(1);
	m_speed->setMinimum(1);

	QObject::connect(m_speed, QOverload<int>::of(&QSpinBox::valueChanged), [=] (int i) {
			std::unique_lock<std::mutex> lock {s_m};
			s_speed = i;
	});

	m_apply = new QPushButton("Apply new change (F2)");
	m_apply->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QObject::connect(m_apply, &QPushButton::clicked, [this] {
		this->testIt();
	});

	m_demo = new QPushButton("Render a demo");
	m_demo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QObject::connect(m_demo, &QPushButton::clicked, [this] {
		this->renderVideoJson();
	});


	m_save = new QPushButton("Save the project");
	m_save->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QObject::connect(m_save, &QPushButton::clicked, [this] {
		this->saveAllSettings();
	});

	m_pause = new QPushButton("Start or Pause");
	m_pause->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QObject::connect(m_pause, &QPushButton::clicked, [this] {
		this->pause();
	});

	m_chessFormSettings->addWidget(new QLabel("The frameRate: "), 1, 1, 1, 1);
	m_chessFormSettings->addWidget(m_frameRate, 1, 2, 1, 1);

	m_chessFormSettings->addWidget(new QLabel("The default Time: "), 2, 1, 1, 1);
	m_chessFormSettings->addWidget(m_defaultTime, 2, 2, 1, 1);

	m_chessFormSettings->addWidget(new QLabel("The speed: "), 3, 1, 1, 1);
	m_chessFormSettings->addWidget(m_speed, 3, 2, 1, 1);


	m_chessFormSettings->addWidget(m_apply, 4, 1, 2, 1);
	m_chessFormSettings->addWidget(m_demo, 4, 2, 2, 1);

	m_chessFormSettings->addWidget(m_save, 6, 1, 2, 1);
	m_chessFormSettings->addWidget(m_pause, 6, 2, 2, 1);

	/// Final
	
	m_chessMainLayout->addLayout(m_chessRight);
	m_chessRight->addWidget(m_imgDisplayLabel);
	m_chessRight->addLayout(m_chessFormSettings);
	m_chessMainLayout->addLayout(m_chessJsonLayout);

	setTextValue();
	setupNewImage();
	//setTextValue();

	//addNewSlide();
}

void ChessWindow::keyPressEvent(QKeyEvent *event) {
	if(event->key() == Qt::Key_F1) {
		compileIt();
	} else if(event->key() == Qt::Key_F2) {
		testIt();
	} else if(event->key() == Qt::Key_F3) {
		nextMove();
	} else if(event->key() == Qt::Key_F4) {
		backMove();
	} else if(event->key() == Qt::Key_F5) {
		addText("\"fcam\": ");
	} else if(event->key() == Qt::Key_F6) {
		addText("\"cam\" : [");
	} else if(event->key() == Qt::Key_F7) {
		addText("\"time\": \"");
	} else if(event->key() == Qt::Key_F8) {
		addText("\"sound\": [\"");
	} else if(event->key() == Qt::Key_F9) {
		addText("\"GreenBackground\": [\"");
	} else if(event->key() == Qt::Key_F10) {
		addText("\"Voice\": [\"");
	}
}


void ChessWindow::resizeEvent(QResizeEvent *event) {
	//std::cout << size().width() << ' ' << size().height() << std::endl;
	
	m_resize = std::async(std::launch::async, [this] {
		std::unique_lock<std::mutex> lck{s_m};

		continueAlive = 3;
		s_window_size = size();
		lck.unlock();
	});
}

void ChessWindow::videoMaker(std::vector<cv::Mat>* s_image, short *continueAlive, QPointer<QLabel> *m_imgDisplayLabel) {

	using namespace std::chrono_literals;
	std::unique_lock<std::mutex> lck{s_m};

	sf::Music music;
	std::vector<QPixmap> s_qPixmap;


	do {
		const std::chrono::milliseconds wait { 1000/frameRateCurrent };
		const std::vector<PlaySound> musicVec = RenderImage::getAudioVec();

		*continueAlive = 0;

		s_qPixmap.clear();

		std::size_t finalSize = s_window_size.height() - SIZE_GRID_LAYOUT;

		if (finalSize - IDE_MIN_SPACE  > s_window_size.width()) {
			finalSize = s_window_size.width() - IDE_MIN_SPACE;
		}

		for (const auto &img : *s_image) {
			cv::Mat outImg, outImg2;

			cv::cvtColor(img, outImg2, cv::COLOR_BGR2RGB);


			cv::resize(outImg2, outImg, cv::Size(finalSize, finalSize));

			s_qPixmap.push_back(QPixmap::fromImage(
							QImage(outImg.data, outImg.cols, outImg.rows,
								outImg.step, QImage::Format_RGB888)));
		}

		if (s_qPixmap.size() == 1) {
			(*m_imgDisplayLabel)->setPixmap(s_qPixmap[0]);
			//return;
			//
		}

		/// Sound part


		std::size_t musIndex {0};
		std::size_t loop {0};


		while (*continueAlive == 0) {
			
			lck.unlock();
			for (float c_index {0}; c_index < s_qPixmap.size() && s_qPixmap.size() != 1; c_index += s_speed) {

				loop = static_cast<std::size_t>(c_index);

				std::this_thread::sleep_for(wait);

				if (musIndex != musicVec.size()) {

					if (static_cast<double>(musicVec[musIndex].startT)/MRATE*frameRateCurrent <= loop) {
						music.stop();
						if (music.openFromFile(musicVec[musIndex].filename)) {
							music.play();
						} else {
							std::cerr << "Can't open " << musicVec[musIndex].filename << std::endl;
						}
						musIndex++;
					}

				}

				(*m_imgDisplayLabel)->setPixmap(s_qPixmap[loop]);
				
				lck.lock();

				if (*continueAlive != 0) {
					lck.unlock();
					break;
				} else {
					lck.unlock();
					if (c_index == s_qPixmap.size() - 1) {
						std::this_thread::sleep_for(1s);
					}
				}
			}

			musIndex = 0;
			
			lck.lock();
		}
	} while (*continueAlive == 3);
}

void ChessWindow::textChange() {
}

QString ChessWindow::getMoveMoment() {
	QString c {QString::number(current)};

	if (type == 0) {
		c += "start";
	} else if (type == 1) {
		c += "middle";
	} else if (type == 2) {
		c += "end";
	}

	return c;
}

bool ChessWindow::compileIt() {
	const std::string key { getMoveMoment().toStdString() };
	const std::string value {"[" + m_jsonValue->toPlainText().toStdString() + "]"};

	if (value != "") {
		try {
			allSettings["render"][key] = json::parse(value);

			if (!checkJson(allSettings["render"][key])) {
				allSettings["render"][key] = json::parse("[]");
				return false;
			}
 		} catch (const std::exception &e) {
			m_jsonError->setText(e.what());
			m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
			return false;
		}
	}
	
	m_jsonError->setText("Ok !");
	m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(0, 255, 0);}"));
	return true;
}

bool ChessWindow::checkJson(json &j) {
	for (std::size_t i {0}; i < j.size(); i++) {
		if (!j[i].contains("fcam")) {
			m_jsonError->setText("Miss fcam in the " + QString::number(i) + " array");
			m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
			return false;
		} else if (!j[i].contains("cam")) {
			m_jsonError->setText("Miss cam in the " + QString::number(i) + " array");
			m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
			return false;
		} /**else if (!j[i].contains("time")) {
			m_jsonError->setText("Miss time in the " + QString::number(i) + " array");
			m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
			return false;
		} //else if (j[i].contains("time") **/

	}

	return true;
}

void ChessWindow::saveJson(std::string_view str_j) {
	const std::string key { getMoveMoment().toStdString() };
	allSettings["render"][key] = json::parse(str_j);
}

void ChessWindow::nextMove() {
	if (!compileIt())
		return;

	type++;

	if (type == 3) {
		type = 0;
		current++;
	}

	m_textJson->setText("The json command: " + getMoveMoment());
	m_jsonError->setText("");
	//m_jsonValue->setText("");
	
	setTextValue();

	setupNewImage();
}

void ChessWindow::backMove() {
	if (!compileIt())
		return;

	if (type == 0 && current == 0) {
		return;
	}

	type--;
	
	if (type == -1) {
		type = 2;
		current--;
	}

	m_textJson->setText("The json command: " + getMoveMoment());
	setTextValue();

	setupNewImage();
}

void ChessWindow::setTextValue() {
	const std::string key { getMoveMoment().toStdString() };

	if (allSettings["render"].contains(key)) {
		QString text = QString::fromStdString(allSettings["render"][key].dump());
		/**if (text[0] == '[') {
			m_jsonValue->document()->setPlainText(text.mid(1, text.size()-2));
		} else {
			m_jsonValue->document()->setPlainText(text);
		}**/

		m_jsonValue->document()->setPlainText(autoIntentJsonData(text));
	} else {
		m_jsonValue->document()->setPlainText("");
	}
}

QString ChessWindow::autoIntentJsonData(const QString &txt) {
	QString result;

	for (std::size_t iStr {0}; iStr < txt.size(); iStr++) {
		if (iStr == 0 && txt[0] == '[') {
			continue;
		} else if (iStr == txt.size() - 1 && txt.at(iStr) == ']') {
			continue;
		} else if (txt.at(iStr) == ':') {
			result += ": ";//txt.at(iStr) + QString::fromStdString("\n\t");
		} else if (txt.at(iStr) == '}') {
			result +=  "\n}";//+ QString::fromStdString("\n\t");
		} else {
			if (txt.at(iStr) == '{') {
				result += txt.at(iStr) + QString::fromStdString("\n\t");
			} else if (iStr < txt.size() - 1) {
				if (txt.at(iStr) == ',') {
					if (txt.at(iStr+1) == '{') {
						result += ",\n";
					} else if (txt.at(iStr+1) == QString("\"")) {
						result += txt.at(iStr) + QString::fromStdString("\n\t");
					} else {
						result += txt.at(iStr) + QString::fromStdString(" ");
					}
				} else {
					result += txt.at(iStr);
				}
			
			} else {
				result += txt.at(iStr);
			}
		}
	}

	return result;

}

void ChessWindow::testIt() {
	if (!compileIt())
		return;

	std::unique_lock<std::mutex> lck{s_m};
	json value {json::parse("[" + m_jsonValue->toPlainText().toStdString() + "]")};


	try {
		m_image = img.getMoveIDE(value[0], current, type, m_frameRate->value(), m_defaultTime->value());
 	} catch (const std::exception &e) {
			m_jsonError->setText(e.what());
			m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
			return;
	}


	if (continueAlive != 2)
		continueAlive = 3;
	else {
		m_function = std::async(std::launch::async, videoMaker, &m_image, &continueAlive, &m_imgDisplayLabel);
	}

	s_window_size = size();
	lck.unlock();
}


void ChessWindow::addText(QString txt) {
	m_jsonValue->textCursor().insertText(txt);
}

void ChessWindow::setupNewImage() {
	std::unique_lock<std::mutex> lck{s_m};
	saveAllSettings();

	std::string key = getMoveMoment().toStdString();
	
	if (allSettings["render"].contains(key)) {
		json &j = allSettings["render"][key];

		try {
			m_image = img.getMoveIDE(allSettings["render"][key], current, type, m_frameRate->value(),
					m_defaultTime->value());
		} catch (const std::exception &e) {
			m_jsonError->setText(e.what());
			m_jsonError->setStyleSheet(QStringLiteral("QLabel{color: rgb(255, 0, 0);}"));
			return;
		}

	} else {
		json j = json::parse("[]");
		m_image = img.getMoveIDE(j, current, type,  m_frameRate->value(), m_defaultTime->value());
	}

	s_window_size = size();

	if (continueAlive != 2)
		continueAlive = 3;
	else {
		m_function = std::async(std::launch::async, videoMaker, &m_image,
				&continueAlive, &m_imgDisplayLabel);
	}

	lck.unlock();

	/**
	if (continueAlive != 2)
		m_function.get();
	
	m_function = std::async(std::launch::async, videoMaker, &m_image, &continueAlive, &m_imgDisplayLabel, m_frameRate->value());
	**/
}

void ChessWindow::pause() {
	std::unique_lock<std::mutex> lck{s_m};

	json j = json::parse("[]");
	m_image = img.getMoveIDE(j, current, type,  m_frameRate->value(), m_defaultTime->value());

	s_window_size = size();

	if (continueAlive != 2)
		continueAlive = 3;
	else {
		m_function = std::async(std::launch::async, videoMaker, &m_image,
				&continueAlive, &m_imgDisplayLabel);
	}
}

void ChessWindow::saveAllSettings() {
	if (!compileIt())
		return;

	std::cout << "Save data to " << fileSettings << std::endl;
	std::ofstream outJ {fileSettings.c_str()};
	outJ << std::setw(4) << allSettings << std::endl;
	outJ.close();
}

void ChessWindow::renderVideoJson() {
	QPointer<QDialog> m_dialog = new RenderDialog(img, allSettings["render"], this);
}

ChessWindow::~ChessWindow() {
	std::unique_lock<std::mutex> lck{s_m};
	continueAlive = 1;

	if (compileIt())
		saveAllSettings();
}
