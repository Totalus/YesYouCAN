#ifndef VIEW_CUSTOMTRANSMITTABLE_H
#define VIEW_CUSTOMTRANSMITTABLE_H

#include <QtWidgets>



class CustomTransmitTableView : public QTableView
{
	Q_OBJECT

	public:
		CustomTransmitTableView(QWidget* parent = 0);

	protected:
		void keyPressEvent(QKeyEvent *event);

	signals:
		void enterPressed(QModelIndex index);
		void spacePressed(QModelIndexList indexList);



	private:
};

#endif // VIEW_CUSTOMTRANSMITTABLE_H
