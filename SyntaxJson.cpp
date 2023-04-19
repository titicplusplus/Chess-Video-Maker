#include "SyntaxJson.hpp"
#include <QColor>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
	HighlightingRule rule;

	closerFormat.setForeground(QColor(83, 170, 177));


	numberFormat.setForeground(QColor(200, 136, 86));

	rule.pattern = QRegularExpression(QStringLiteral("\\d"));
	rule.format = numberFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegularExpression(QStringLiteral("\\d.\\d"));
	rule.format = numberFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegularExpression(QStringLiteral("(true|false)"));
	rule.format = numberFormat;
	highlightingRules.append(rule);

	quotFormat.setForeground(QColor(152, 190, 136));

	rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
	rule.format = quotFormat;
	highlightingRules.append(rule);



	rule.pattern = QRegularExpression(QStringLiteral("(\\{|\\[|\\}|\\]|\"|:|,)"));
	rule.format = closerFormat;
	highlightingRules.append(rule);



	/**rule.pattern = QRegularExpression(QStringLiteral("(GreenBackground|Voice)"));
	rule.format = effectFormat;
	highlightingRules.append(rule);

	timeFormat.setForeground(Qt::darkGreen);

	rule.pattern = QRegularExpression(QStringLiteral("(time|Effect|effect)"));
	rule.format = timeFormat;
	highlightingRules.append(rule);

	soundFormat.setForeground(Qt::blue);

	rule.pattern = QRegularExpression(QStringLiteral("(sound)"));
	rule.format = soundFormat;
	highlightingRules.append(rule);

	camFormat.setForeground(Qt::darkRed);

	rule.pattern = QRegularExpression(QStringLiteral("(cam|fcam)"));
	rule.format = camFormat;
	highlightingRules.append(rule);**/



}

void Highlighter::highlightBlock(const QString &text)
{
	for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}

	setCurrentBlockState(0);
}
