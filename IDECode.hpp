#pragma once
#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit
{
	public:
		CodeEditor(QWidget *parent = nullptr);

	    	void lineNumberAreaPaintEvent(QPaintEvent *event);
	    	int lineNumberAreaWidth();

	protected:
	    	void resizeEvent(QResizeEvent *event) override;

	private slots:
	    	void updateLineNumberAreaWidth(int newBlockCount);
	    	void highlightCurrentLine();
	    	void updateLineNumberArea(const QRect &rect, int dy);

	private:
	    	QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget
{
	public:
		LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
			{}

		QSize sizeHint() const override
		{
			return QSize(codeEditor->lineNumberAreaWidth(), 0);
		}

	protected:
		void paintEvent(QPaintEvent *event) override
		{
			codeEditor->lineNumberAreaPaintEvent(event);
		}

	private:
		CodeEditor *codeEditor;
};
