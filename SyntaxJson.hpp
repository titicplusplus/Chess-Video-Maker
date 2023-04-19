#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression> 
#include <QTextCharFormat> 
#include <QTextDocument>

class Highlighter : public QSyntaxHighlighter
{
    //Q_OBJECT

	public:
		Highlighter(QTextDocument *parent = 0);

	protected:
		void highlightBlock(const QString &text) override;

	private:
		struct HighlightingRule
		{
			QRegularExpression pattern;
			QTextCharFormat format;
		};

		QVector<HighlightingRule> highlightingRules;
	
		QTextCharFormat numberFormat;
		QTextCharFormat quotFormat;
		QTextCharFormat closerFormat;
};
