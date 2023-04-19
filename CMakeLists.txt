cmake_minimum_required(VERSION 3.10)

project(ChessVideoMaker VERSION 0.90)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SYSTEM_NAME Linux)

add_executable(
	ChessVideoMaker
	main.cpp
	IDECode.cpp
	SyntaxJson.cpp
	ChessDialog.cpp
	ChessWindow.cpp
	getpuzzle.cpp
	ChessParser.cpp
	DetectEvent.cpp
	ChessRenderImage.cpp
	#OpenMusic.cpp
	default.cpp
	ChessAutoMaker.cpp
	StockFishUCI.cpp
	Effect/effect.cpp
	Effect/GreenBackground.cpp
	Effect/voice.cpp
	camera.cpp
	SaveMusic.cpp
	JsonCommand.cpp
	Event/event.cpp
	Event/rating_move.cpp
	Event/Check.cpp
	Event/multipleAttack.cpp
	Event/solution.cpp
	Event/Mistake.cpp
	Event/textGeneration.cpp
	Event/videoGeneration.cpp
	Ressources.cpp
)

find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(
	ChessVideoMaker
	stdc++fs
	curl
	curlpp
	sfml-audio
	pthread
	sfml-audio
	${OpenCV_LIBS}
	Qt5::Widgets
	Qt5::Core
	Qt5::Gui
	#avcpp
	#avutil
	atomic
)

install(TARGETS ChessVideoMaker DESTINATION bin)
