#include "widget_cantrace.h"

// Those includes in the header file cause compiling issues
#include "document.h"
#include "application.h"

CanTraceWidget::CanTraceWidget(MainWindow *parent, Document *document) : AbstractTabWidget(parent, document)
{
	m_mainWindow = parent;

	m_dbc = 0;
	m_msgWidget = new CanMsgWidget(m_dbc, this);
	m_msgWidget->hide(); // Hide when no DBC associated

	m_device = 0;

	m_replayIndex = 0;

	m_replayTimer = new QTimer(this);
	m_replayTimer->setSingleShot(true);
	m_replayTimer->setTimerType(Qt::PreciseTimer);
	connect(m_replayTimer, SIGNAL(timeout()), this, SLOT(sendNextMessage()));

	// Model
	m_model = new CanTraceModel(this);

	m_filter_model = new CustomFilterModel(this);
	m_filter_model->setSourceModel(m_model);

	// View
	m_view = new QTableView(this);
	m_view->verticalHeader()->setDefaultSectionSize(18); // Row height
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);

	m_view->setModel(m_filter_model);

	m_view->setColumnWidth(TRACE_COL_NUMBER, 60);
	m_view->setColumnWidth(TRACE_COL_TIMESTAMP, 140);
	m_view->setColumnWidth(TRACE_COL_ID, 100);
	m_view->setColumnWidth(TRACE_COL_TYPE, 80);
	m_view->setColumnWidth(TRACE_COL_DATA_LEN, 50);
	m_view->setColumnWidth(TRACE_COL_DATA, 220);

	//connect(m_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChangedSlot(QModelIndex)));

	createActions(); // Menu and toolbar actions

	// Menu
	m_traceMenu = new QMenu("Trace", this);
	populateMenu();

	// Tool bar
	m_toolbar = new QToolBar(this);
	populateToolBar();

	m_filterHelpAct = new QAction(QIcon(":/icons/questionmark"), "Help");
	connect(m_filterHelpAct, SIGNAL(triggered(bool)), this, SLOT(showFilterHelp()));
	m_filterBar = new FilterLineEdit(this);
	connect(m_filterBar, SIGNAL(returnPressed()), this, SLOT(applyFilter()));
	connect(m_filterBar, SIGNAL(textEdited(QString)), this, SLOT(filterTextChanged(QString)));

	m_filterToolBar = new QToolBar(this);
	m_filterToolBar->addWidget(new QLabel("Filter expression : "));
	m_filterToolBar->addWidget(m_filterBar);
	m_filterToolBar->addAction(m_filterHelpAct);
	m_filterToolBar->hide();

	createStatusBar(); // Infostatus bar

	QSplitter *splitter = new QSplitter(Qt::Vertical);
	splitter->addWidget(m_view);
	splitter->addWidget(m_msgWidget);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_toolbar);
	layout->addWidget(m_filterToolBar);
	layout->addWidget(splitter);

	setLayout(layout);

	connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateStatusBar()));

	layout->addWidget(m_statusBar);
	layout->setSpacing(1);
	layout->setContentsMargins(6,6,6,0);

	QTimer *timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()), this, SLOT(updateStatusBar()));
	timer->start(3500); // Update status bar each 3 seconds
}

CanTraceWidget::~CanTraceWidget()
{
}

void CanTraceWidget::createStatusBar()
{
	// Create status bar elements

	// Common font
	QFont font = QPushButton().font();
	font.setPointSize(8);

	// Number of messages in the trace
	m_status_nb_msg = new QPushButton(this);
	m_status_nb_msg->setFlat(true);
	m_status_nb_msg->setFont(font);

	// Related DBC
	m_status_dbc = new QPushButton("DBC : None", this);
	m_status_dbc->setFlat(true);
	m_status_dbc->setFont(font);
	connect(m_status_dbc, SIGNAL(clicked(bool)), this, SLOT(setTraceDbc()));

	// Filter related
	m_status_filter = new QPushButton("Filters : 0", this);
	m_status_filter->setFlat(true);
	m_status_filter->setFont(font);
	connect(m_status_filter, SIGNAL(clicked(bool)), this, SLOT(showFilterBar(bool)));

	m_status_device = new QPushButton("Interface : -", this);
	m_status_device->setFlat(true);
	m_status_device->setFont(font);
	connect(m_status_device, SIGNAL(clicked(bool)), this, SLOT(setDevice()));

	// Create status bar
	m_statusBar = new QStatusBar(this);
	m_statusBar->insertWidget(0, m_status_nb_msg);
	m_statusBar->insertWidget(1, m_status_device);
	m_statusBar->insertWidget(2, m_status_dbc);
	m_statusBar->insertWidget(3, m_status_filter);
	m_statusBar->setMaximumHeight(27);

	updateStatusBar();
	updateVisibleActions();
}

void CanTraceWidget::createActions()
{
	// Actions
	m_traceDbcAct = new QAction(QIcon(":/icons/dbc"), "Associate DBC", this);
	connect(m_traceDbcAct, SIGNAL(triggered()), this, SLOT(setTraceDbc()));

	m_saveTraceAct = new QAction(QIcon(":/icons/save"), "Save Trace", this);
	connect(m_saveTraceAct, SIGNAL(triggered(bool)), this, SLOT(saveTrace()));

	m_showFilterBarAct = new QAction(QIcon(":/icons/filter"), "Filters", this);
	m_showFilterBarAct->setCheckable(true);
	connect(m_showFilterBarAct, SIGNAL(triggered(bool)), this, SLOT(showFilterBar(bool)));

	m_exportSignalsAct = new QAction("Export Signals Data", this);
	m_exportSignalsAct->setEnabled(false);
	m_exportSignalsAct->setToolTip("Export specific signals to file. Need to associate to a database first.");
	connect(m_exportSignalsAct, SIGNAL(triggered()), this, SLOT(exportTraceSignals()));

	m_deviceAct = new QAction(QIcon(":/icons/disconnected"), "Link to device", this);
	connect(m_deviceAct, SIGNAL(triggered()), this, SLOT(setDevice()));

	m_startStopCaptureAct = new QAction(QIcon(":/icons/record"), "Capture", this);
	m_startStopCaptureAct->setEnabled(false);
	m_startStopCaptureAct->setCheckable(true);
	connect(m_startStopCaptureAct, SIGNAL(triggered(bool)), this, SLOT(startStopCapture(bool)));

	m_replayTraceAct = new QAction(QIcon(":/icons/play"), "Replay trace", this);
	m_replayTraceAct->setEnabled(false);
	connect(m_replayTraceAct, SIGNAL(triggered(bool)), this, SLOT(replayTrace()));

	m_replayIndexLabelAct = new QAction("Index : 0", this);
	m_replayIndexLabelAct->setEnabled(false);
	connect(m_replayIndexLabelAct, SIGNAL(triggered(bool)), this, SLOT(changeReplayIndex()));
	//m_replayIndexLabelAct->setToolTip("Index of message to be sent next");

	m_useMsgNumberOffset = new QAction("Use message number offset", this);
	m_useMsgNumberOffset->setToolTip("Message number starts by 1/Message number is the interface absolute message");
	m_useMsgNumberOffset->setCheckable(true);
	m_useMsgNumberOffset->setChecked(true);
	connect(m_useMsgNumberOffset, SIGNAL(triggered(bool)), m_model, SLOT(useMessageNumberOffset(bool)));

	m_useTimestampOffset = new QAction("Use timestamp offset", this);
	m_useTimestampOffset->setToolTip("Timstamp starts by 0s/Timestamp is the interface absolute timstamp");
	m_useTimestampOffset->setCheckable(true);
	m_useTimestampOffset->setChecked(true);
	connect(m_useTimestampOffset, SIGNAL(triggered(bool)), m_model, SLOT(useTimestampOffset(bool)));

	m_addGraph = new QAction(QIcon(":/icons/graph"), "Add Graph", this);
	m_addGraph->setToolTip("New graph tab from trace");
	connect(m_addGraph, SIGNAL(triggered(bool)), this, SLOT(addGraph()));
}

void CanTraceWidget::populateToolBar()
{
	m_toolbar->clear();

	m_toolbar->addAction(m_deviceAct);
	m_toolbar->addAction(m_traceDbcAct);
	m_toolbar->addSeparator();
	m_toolbar->addAction(m_startStopCaptureAct);
	m_toolbar->addAction(m_replayTraceAct);
	m_toolbar->addAction(m_replayIndexLabelAct);
	m_toolbar->addSeparator();
	m_toolbar->addAction(m_saveTraceAct);
	m_toolbar->addAction(m_showFilterBarAct);
	m_toolbar->addAction(m_addGraph);

	if(!m_document->isSaved())
		m_toolbar->addAction(m_exportSignalsAct);
}

void CanTraceWidget::populateMenu()
{
	m_traceMenu->clear();

	m_traceMenu->addAction(m_saveTraceAct);
	m_traceMenu->addAction(m_showFilterBarAct);
	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_traceDbcAct);
	m_traceMenu->addAction(m_exportSignalsAct);

	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_deviceAct);
	m_traceMenu->addAction(m_startStopCaptureAct);
	m_traceMenu->addAction(m_replayTraceAct);

	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_useMsgNumberOffset);
	m_traceMenu->addAction(m_useTimestampOffset);

	m_traceMenu->addSeparator();
	m_traceMenu->addAction(m_addGraph);

}

void CanTraceWidget::updateVisibleActions()
{
	if(m_document->isSaved())
	{
		m_startStopCaptureAct->setVisible(false);
		m_exportSignalsAct->setVisible(true);
		m_replayTraceAct->setVisible(true);
		m_replayIndexLabelAct->setVisible(true);
	}
	else
	{
		m_startStopCaptureAct->setVisible(true);
		m_exportSignalsAct->setVisible(false);
		m_replayTraceAct->setVisible(false);
		m_replayIndexLabelAct->setVisible(false);
	}
}


CanTraceModel* CanTraceWidget::model()
{
	return m_model;
}

void CanTraceWidget::setDbc(DbcModel *dbc)
{
	m_dbc = dbc;
	m_model->setDbcModel(dbc);
	m_msgWidget->setDbcModel(dbc);

	m_filterBar->updateCompletionLists(dbc);

	if(m_dbc == 0)
	{
		m_msgWidget->hide();
		m_view->setColumnWidth(TRACE_COL_ID, 100); // Size when no dbc attributed
		m_status_dbc->setText("DBC: None");
		m_exportSignalsAct->setEnabled(false);
		m_exportSignalsAct->setToolTip("Export specific signals to file.");
		m_traceDbcAct->setIcon(QIcon(":/icons/dbc"));
	}
	else
	{
		m_msgWidget->show();
		m_view->setColumnWidth(TRACE_COL_ID, 260); // Size when dbc attributed
		m_status_dbc->setText("DBC : " + dbc->getFileName());
		m_exportSignalsAct->setEnabled(true);
		m_traceDbcAct->setIcon(QIcon(":/icons/database_ok"));
	}
}

void CanTraceWidget::loadTraceFile_PCAN(QString file_name)
{
	m_model->loadTraceFile_PCAN(file_name);

	QStringList parsing_errors = m_model->getErrors();

	if (!parsing_errors.isEmpty())
	{
		QMessageBox::warning(this,"Parsing error(s)", parsing_errors.join('\n'));
	}

	updateStatusBar();
	updateVisibleActions();
}

void CanTraceWidget::loadTraceFile_BusMaster(QString file_name)
{
	m_model->loadTraceFile_BusMaster(file_name);

	QStringList parsing_errors = m_model->getErrors();

	if (!parsing_errors.isEmpty())
	{
		QMessageBox::warning(this,"Parsing error(s)", parsing_errors.join('\n'));
	}

	updateStatusBar();
	updateVisibleActions();
}

void CanTraceWidget::loadTraceFile_WiresharkExport(QString file_name)
{
	m_model->loadTraceFile_WiresharkExport(file_name);

	QStringList parsing_errors = m_model->getErrors();

	if (!parsing_errors.isEmpty())
	{
		QMessageBox::warning(this,"Parsing error(s)", parsing_errors.join('\n'));
	}

	updateStatusBar();
	updateVisibleActions();
}


void CanTraceWidget::selectionChangedSlot(QModelIndex current)
{
	QModelIndex index = m_filter_model->index(current.row(), TRACE_COL_NUMBER);

	// Update msg info in information widget
	if(current.isValid())
		m_msgWidget->setMessage(m_model->getMessage(m_filter_model->data(index,Qt::UserRole).toInt()));

	// Update highlighting of selected object
	/*
	if(current.isValid())
		m_model->setCurrentSelectedMsgId(m_model->getMessage(m_filter_model->data(index,Qt::UserRole).toInt()).id);
	else
		m_model->setCurrentSelectedMsgId(-1);
	//*/
}

QMenu* CanTraceWidget::specificMenu()
{
	return m_traceMenu;
}


void CanTraceWidget::saveTrace()
{
	QFileDialog dialog(this, "Save trace", QString(), "PEAK v1.1 (*.trc);;PEAK v2.0 (*.trc);;CSV (*.csv)");
	dialog.setAcceptMode(QFileDialog::AcceptSave);

	if(dialog.exec() == QFileDialog::Rejected)
		return;

	if(dialog.selectedFiles().isEmpty())
		return;

	QString file_path = dialog.selectedFiles().at(0);

	if(file_path.isEmpty())
		return;


	if(dialog.selectedNameFilter() == "PEAK v1.1 (*.trc)")
	{
		if(!file_path.endsWith(".trc"))
			file_path += ".trc";

		QFile file(file_path);
		file.open(QIODevice::WriteOnly | QIODevice::Text);

		QTextStream stream(&file);

		stream << ";$FILEVERSION=1.1\n";
		stream << ";$STARTTIME=0\n";
		stream << ";\n";
		stream << ";   " + file_path + "\n";
		stream << ";\n";
		stream << ";\n";
		stream << ";   Message Number\n";
		stream << ";   |         Time Offset (ms)\n";
		stream << ";   |         |        Type\n";
		stream << ";   |         |        |        ID (hex)\n";
		stream << ";   |         |        |        |     Data Length Code\n";
		stream << ";   |         |        |        |     |   Data Bytes (hex) ...\n";
		stream << ";   |         |        |        |     |   |\n";
		stream << ";---+--   ----+----  --+--  ----+---  +  -+ -- -- -- -- -- -- --\n";


		// Progress bar (TODO : marche pas comme il faut...)
		QProgressDialog progress("Saving trace file", "Cancel", 0, m_model->count(), this);
		progress.setWindowModality(Qt::WindowModal);
		//progress.show();

		int i;
		for(i = 0; i < m_model->count(); i++)
		{
			progress.setValue(i);

			//     2)       612.0  Rx         0003  8  0E 9D 0E 9D 0E 9D 0E A0
			CanTraceMessage msg = m_model->getMessage(i);
			QString line;
			line.sprintf("%6d) %11.1f  " ,i+1, msg.timestamp*1000); // Timestamp in ms
			line += msg.type;
			line.append(QString().fill(' ',32 - line.length()));
			line.append(QString().sprintf("%04x",msg.id).toUpper());
			line.append("  ");
			line.append(QString::number(msg.data.length()));

			for (int j = 0; j < msg.data.size(); j++)
			{
				if(j == 0)
				{
					line.append("  ");
					line.append(QString().sprintf("%02x",msg.data.at(j)).toUpper());
				}
				else
				{
					line.append(" ").append(QString().sprintf("%02x",msg.data.at(j)).toUpper());
				}
			}
			stream << line << "\n";

			if(progress.wasCanceled())
			{
				file.close();
				file.remove();
				return;
			}

		}

		file.close();
		progress.setValue(i);
		m_document->setSaved(true);
	}
	else
	{
		QMessageBox::warning(this, "Format not supported", "Format not supported yet");
	}

	updateVisibleActions();
}


void CanTraceWidget::setTraceDbc()
{
	DbcModel* dbc = DbcDialog::getDbc(Application::dbcList(), this);

	if(dbc != 0)
	{
		setDbc(dbc);
		emit dbcChanged(dbc);
	}
}


void CanTraceWidget::exportTraceSignals()
{
	// Get existing signals
	QStringList signal_list = m_dbc->getSignalList(); // Existing signals (in database)
	signal_list.sort();


	//
	//	Export Dialog (gather export signals and configuration)
	//

	ExportDialog dialog(this);
	dialog.setItemList(signal_list);

	if(dialog.exec() != QDialog::Accepted)
		return;

	QList<ExportItem_t> export_list = dialog.getSignalsToExport(); // Export signals list

	if(dialog.getExportType() == "csv")
	{
		QStringList export_lines; // Line of exported file

		// Progress bar dialog
		QProgressDialog progress("Exporting signals from trace...", "Stop export",0, m_model->count()*export_list.count(), this);
		progress.setWindowModality(Qt::WindowModal);
		progress.setFixedSize(400,100);
		progress.setWindowTitle("Exporting signals");

		int nb_columns = 0; // Number of columns in the file
		QString sep = dialog.getSeperator();

		for(int i = 0; i < export_list.count(); i++)
		{
			ExportItem_t a = export_list.at(i);
			QString signal_name = a.sigName;

			if(!a.export_timestamp && !a.export_raw && !a.export_phy)
				continue; // Nothing to export

			//progressLabel.setText("Exporting " + signal_name + "' (" + QString::number(i+1) + "/" + QString::number(export_list.count()) + ")");
			progress.setLabelText("Exporting signal '" + signal_name + "' (" + QString::number(i+1) + "/" + QString::number(export_list.count()) + ")");

			// Search signal in database
			if(!m_dbc->containsSignal(signal_name))
			{
				QMessageBox::warning(this, "Unknown signal", "Signal \"" + signal_name + "\" is not present in the associated database.");
				continue;
			}

			// Search message containing this signal
			CanSignal *signal = m_dbc->getSignal(signal_name);
			QList<quint32> msg_id_list = signal->getAssociatedMsgIDs();

			if(msg_id_list.empty())
			{
				QMessageBox::warning(this, "Orphan signal", "No CAN message contains the signal " + signal_name + " (in database)");
				continue;
			}

			// Add first header line (signal name)
			QString line = "";
			if(a.export_timestamp)
				line += sep + signal_name;

			if(a.export_raw)
				line += sep + signal_name;

			if(a.export_phy)
				line += sep + signal_name;

			if(export_lines.count() >= 1)
				export_lines[0] += line;
			else // First line does not exist, create
				export_lines.append(line.remove(0,sep.length()));


			// Add second header line (signal name)
			line = "";
			if(a.export_timestamp)
				line += sep + "Time(s)";

			if(a.export_raw)
				line += sep + "Raw value";

			if(a.export_phy)
				line += sep + "Physical (" + signal->getUnits() + ")";

			if(export_lines.count() >= 2)
				export_lines[1] += line;
			else // First line does not exist, create
				export_lines.append(line.remove(0, sep.length()));


			// For all messages in trace
			int j;
			int export_line_index = 2; // 2 because of two first header lines
			for(j = 0; j < m_model->count(); j++)
			{
				// Get signal value and timestamp
				CanTraceMessage msg_data = m_model->getMessage(j);

				if(msg_id_list.contains(msg_data.id)) // Signal in this message
				{
					CanMessage* msg_def = m_dbc->getMessage(m_model->getMessage(j).id);

					bool ok;
					quint64 raw_value = msg_def->getRawSignalValue(signal_name, msg_data.data, &ok);

					if(ok)
					{
						QString formatted_value = m_dbc->getSignal(signal_name)->formatStr(raw_value);

						// Construct line
						line = "";
						if(a.export_timestamp)
							line += sep + QString::number(msg_data.timestamp);

						if(a.export_raw)
							line += sep + QString::number(raw_value);

						if(a.export_phy)
							line += sep + formatted_value;

						if(export_lines.count() - 1 >= export_line_index) // line exists
						{
							export_lines[export_line_index] += line; // concatenate line
						}
						else // new line
						{
							// fill for previous signals and append line
							line = QString(sep).repeated(nb_columns) + line.remove(0,sep.length());
							export_lines.append(line);
						}
						export_line_index++;
					}
				}

				progress.setValue(j + i*m_model->count());

				if(progress.wasCanceled())
					break;
			}

			if(progress.wasCanceled())
			{
				QMessageBox::information(this, "Export signals interrupted", "Signals that have been processed will be written to file, last signal might not be completely processed");
				break;
			}

			for(;export_line_index < export_lines.count(); export_line_index++)
			{
				//fill column with empty data
				int nb_sep = export_lines.at(export_line_index).count(sep);
				export_lines[export_line_index] += QString(sep).repeated(nb_columns - nb_sep - 1);
			}

			nb_columns += a.hint; // number of columns written
			progress.setValue(m_model->count() * export_list.count());
		}

		progress.close();

		//
		//	Get output file name
		//

		// File (get name, open, ...)
		QString file_name = QFileDialog::getSaveFileName(this, "Save File", QString() , "*.csv");

		if(file_name.isEmpty())
			return;

		if(!file_name.contains('.'))
			file_name += ".csv";


		QFile file(file_name);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::critical(this, "Error : File access", "Can't create specified file");
			return;
		}

		QTextStream fstream(&file);

		QProgressDialog write_progress("Writing export file","Cancel", 0, export_lines.count(), this);
		write_progress.setWindowModality(Qt::WindowModal);

		int k;
		for(k = 0; k < export_lines.count(); k++)
		{
			write_progress.setValue(k);
			fstream << export_lines.at(k) << endl;

			if(write_progress.wasCanceled())
			{
				QMessageBox::warning(this, "Write to file interrupted", "File partially written : " + QString::number(k) + " lines out of " + QString::number(export_lines.count()));
				break;
			}
		}
		write_progress.setValue(k);

		file.close();
	}
	else if(dialog.getExportType() == "matlab")
	{
		//
		//	Get output file name
		//

		// File (get name, open, ...)
		QString file_name = QFileDialog::getSaveFileName(this, "Save File", QString() , "*.m");

		if(file_name.isEmpty())
			return;

		if(!file_name.contains('.'))
			file_name += ".m";

		QFile file(file_name);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::critical(this, "Error : File access", "Can't create specified file");
			return;
		}

		QTextStream fstream(&file);


		//
		// Generate output
		//

		// Add first header line (signal name)
		QString line;
		line += "% -------------------------------------------------------------------\n";
		line +=	"%  Generated by YesYouCan on " + QTime::currentTime().toString() + "\n";
		line += "%  Frome trace " + m_document->fileName() + "\n";
		line += "% -------------------------------------------------------------------\n\n";

		fstream << line; // Write to file

		// Progress bar dialog
		QProgressDialog progress("Exporting signals from trace...", "Stop export",0, m_model->count()*export_list.count(), this);
		//progress.setWindowModality(Qt::WindowModal);
		progress.setFixedSize(400,100);
		progress.setWindowTitle("Exporting signals");

		for(int i = 0; i < export_list.count(); i++)
		{
			ExportItem_t a = export_list.at(i);
			QString signal_name = a.sigName;

			if(!a.export_timestamp && !a.export_raw && !a.export_phy)
				continue; // Nothing to export

			progress.setLabelText("Exporting signal '" + signal_name + "' (" + QString::number(i+1) + "/" + QString::number(export_list.count()) + ")");

			// Search signal in database
			if(!m_dbc->containsSignal(signal_name))
			{
				QMessageBox::warning(this, "Unknown signal", "Signal \"" + signal_name + "\" is not present in the associated database.");
				continue;
			}

			// Search message containing this signal
			CanSignal *signal = m_dbc->getSignal(signal_name);
			QList<quint32> msg_id_list = signal->getAssociatedMsgIDs();

			if(msg_id_list.empty())
			{
				QMessageBox::warning(this, "Orphan signal", "No CAN message contains the signal " + signal_name + " (in database)");
				continue;
			}


			QString time_line, raw_line, physical_line, units_line;

			// Prepare structure
			time_line = signal->getName() + ".time = [";
			raw_line = signal->getName() + ".raw = [";
			physical_line = signal->getName() + ".physical = [";
			units_line = signal->getName() + ".units = '" + signal->getUnits() + "';";

			// For all messages in trace
			int j;
			bool first = true;
			for(j = 0; j < m_model->count(); j++)
			{
				// Get signal value and timestamp
				CanTraceMessage msg_data = m_model->getMessage(j);

				if(msg_id_list.contains(msg_data.id)) // Signal in this message
				{
					CanMessage* msg_def = m_dbc->getMessage(m_model->getMessage(j).id);

					bool ok;
					quint64 raw_value = msg_def->getRawSignalValue(signal_name, msg_data.data, &ok);

					if(ok)
					{
						QString formatted_value = m_dbc->getSignal(signal_name)->formatStr(raw_value);

						QString sep = "; ";

						if(first)
						{
							sep = "";
							first = false;
						}

						// Construct line
						if(a.export_timestamp)
							time_line += sep + QString::number(msg_data.timestamp);

						if(a.export_raw)
							raw_line += sep + QString::number(raw_value);

						if(a.export_phy)
						{
							bool ok;
							formatted_value.toDouble(&ok);

							if(ok)
								physical_line += sep + formatted_value;
							else
							{
								QMessageBox::warning(this, "Physical value format", "The physical value for signal " + signal->getName() + " is not a valid number. Physical value for this signal will not be exported");
								a.export_phy = false;
							}
						}
					}
				}

				//progress.setValue(j + i*m_model->count());

				if(progress.wasCanceled())
					break;
			}

			// Write lines to file
			time_line += "];";
			raw_line += "];";
			physical_line += "];";

			if(a.export_timestamp)
				fstream << time_line << endl;
			if(a.export_raw)
				fstream << raw_line << endl;
			if(a.export_phy)
				fstream << physical_line << endl;
			if(a.hint != 0)
				fstream << units_line << endl << endl;

			if(progress.wasCanceled())
			{
				QMessageBox::information(this, "Export signals interrupted", "Signals that have been processed will be written to file, last signal might not be completely processed");
				break;
			}

			progress.setValue(m_model->count() * export_list.count());
		}

		progress.close();

		file.close();
	}
}

void CanTraceWidget::showFilterBar(bool checked)
{
	m_filterToolBar->setVisible(checked);
}

void CanTraceWidget::setDevice()
{
	QStringList iface_str;

	for(int i = 0; i < Application::interfaceList().count(); i++)
	{
		iface_str.append(Application::interfaceList().at(i)->getIdentifier() + " (" + Application::interfaceList().at(i)->getDescription() + ")");
	}

	if(iface_str.isEmpty())
	{
		QMessageBox::information(this, "No Interface", "There is no available interfaces configured. Create a new interface first to associate the interface");
		return;
	}

	ItemDialog dialog(this, "Select interface");
	dialog.setWindowTitle("Existing interfaces");
	dialog.setItemList(iface_str);
	dialog.setSelectionMode(QAbstractItemView::SingleSelection);
	dialog.resize(500, 300);

	if(dialog.exec() == ItemDialog::Accepted)
	{
		// Set device
		QStringList selection = dialog.getSelectedItems();

		if(!selection.isEmpty())
		{
			// Set the corresponding interface
			for(int i = 0; i < iface_str.count(); i++)
			{
				if(selection.at(0) == iface_str.at(i))
				{
					m_device = Application::interfaceList().at(i);	// Set the new device
					m_device->getDocument()->addChildren(document()); // Add this widget document to the new device document list

					// Update button states
					m_deviceAct->setIcon(QIcon(":/icons/connected"));
					m_startStopCaptureAct->setEnabled(true);
					m_replayTraceAct->setEnabled(true);
					m_replayIndexLabelAct->setEnabled(true);
					updateStatusBar();

					return;
				}
			}
		}
	}
}

void CanTraceWidget::startStopCapture(bool checked)
{
	if(m_device == 0)
		return;

	if(!m_device->isCapturing())
	{
		if(!m_device->setCapture(true))
		{
			// TODO : Message box explaining error
			return;
		}
	}

	if(checked)
	{
		connect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(addMessage(CanTraceMessage)), Qt::UniqueConnection);
	}
	else
	{
		disconnect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(addMessage(CanTraceMessage)));
	}

	m_deviceAct->setEnabled(!checked);
	m_saveTraceAct->setEnabled(!checked);
	m_exportSignalsAct->setEnabled(!checked);

	/*
	if(!m_device->isCapturing())
	{
		if(!m_device->setCapture(true))
		{
			// TODO : Message box explaining error
			return;
		}
	}
	else
	{
		if(!m_device->setCapture(false))
		{
				// TODO : Message box explaining error
				return;
		}
	}

	connect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(addMessage(CanTraceMessage)), Qt::UniqueConnection);
	m_startStopCaptureAct->setEnabled(false);
	//m_stopCaptureAct->setEnabled(true);
	*/
}

/*
void CanTraceWidget::stopCapture()
{
	if(m_device == 0)
		return;

	//m_device->setCapture(false); // No need to disable hardware capture

	disconnect(m_device, SIGNAL(newCanMessage(CanTraceMessage)), m_model, SLOT(addMessage(CanTraceMessage)));
	m_startStopCaptureAct->setEnabled(true);
//	m_stopCaptureAct->setEnabled(false);

	updateStatusBar();
}
*/

void CanTraceWidget::updateStatusBar()
{
	m_status_nb_msg->setText("Message : " + QString::number(m_model->count()));

	if(m_device != 0)
		m_status_device->setText("Interface : " + m_device->getIdentifier() + " (" + QString::number(m_device->getRxRate()) + " msg/s)");
	else
		m_status_device->setText("Interface : -");
}

QToolBar* CanTraceWidget::specificToolBar()
{
	return 0; // TODO : implement or remove
}


void CanTraceWidget::replayTrace()
{
	if(m_device == 0)
		return;

	if(m_replayTimer->isActive()) // Stop replay (already active)
	{
		m_replayTraceAct->setIcon(QIcon(":/icons/play"));
		m_replayTimer->stop();
		m_replayIndexLabelAct->setDisabled(false);
	}
	else // Start replay (not active)
	{
		m_replayTraceAct->setIcon(QIcon(":/icons/pause"));
		m_replayTimer->setInterval(0);
		m_replayTimer->start();
		m_replayIndexLabelAct->setDisabled(true);
	}

}

void CanTraceWidget::sendNextMessage()
{
	if(m_device == 0)
		return;

	if(m_replayIndex >= m_model->rowCount())
	{
		m_replayIndex = 0;
	}

	CanTraceMessage msg = m_model->getMessage(m_replayIndex);

	if(m_replayIndex + 1 < m_model->rowCount()) // Not the last message
	{
		CanTraceMessage next_msg = m_model->getMessage(m_replayIndex + 1);

		// Update timer interval for next message, start now not to add delay
		//if(next_msg.timestamp > msg.timestamp) // TODO : remove ?
			m_replayTimer->setInterval((next_msg.timestamp - msg.timestamp)*1000); // in ms
		//else
		//	m_replayTimer->setInterval(0);

		m_replayTimer->start();
	}
	else
	{
		m_replayTraceAct->setIcon(QIcon(":/icons/play"));
		m_replayIndexLabelAct->setDisabled(false);
	}

	// Send message
	m_device->writeMessage(msg);

	m_replayIndexLabelAct->setText("Index : " + QString::number(m_replayIndex));
	m_replayIndex++;
}


void CanTraceWidget::changeReplayIndex()
{
	if(m_replayIndex == m_model->rowCount())
	{
		m_replayIndex = 0;
		m_replayIndexLabelAct->setText("Index : 0");
		return;
	}

	bool ok;
	int index = QInputDialog::getInt(this, "Select message index", "Select the index of message from which to start the trace replay.", m_replayIndex, 0, m_model->rowCount() - 1, 1, &ok);

	if(ok == true)
	{
		m_replayIndex = index;
		m_replayIndexLabelAct->setText("Index : " + QString::number(index));
	}
}

void CanTraceWidget::applyFilter()
{
	FilterExpression exp(m_filterBar->text(), m_dbc);

	if(m_filterBar->text().isEmpty())
	{
		m_filter_model->setExpression("", m_dbc);
		m_filterHelpAct->setIcon(QIcon(":/icons/questionmark"));
		m_showFilterBarAct->setIcon(QIcon(":/icons/filter"));
	}
	else if(!exp.isValid())
	{
		QToolTip::showText(m_filterBar->mapToGlobal(QPoint(0, 10)), exp.getError());
		m_filterHelpAct->setIcon(QIcon(":/icons/error"));
	}
	else
	{
		m_filter_model->setExpression(m_filterBar->text(), m_dbc);
		m_filterHelpAct->setIcon(QIcon(":/icons/apply"));
		m_showFilterBarAct->setIcon(QIcon(":/icons/filter_check"));
	}
}


void CanTraceWidget::filterTextChanged(QString text)
{
	if(text.isEmpty())
		m_filterHelpAct->setIcon(QIcon(":/icons/questionmark"));
	else
		m_filterHelpAct->setIcon(QIcon(":/icons/warning"));
}

void CanTraceWidget::showFilterHelp()
{
	QString help_text = "The filtering system is an expression based filtering that allows you to combine filters in a powerful way."
			"A combination of operands and special terms allow you to filter the messages in the trace. You can use parentheses to set priority in the expression.\n\n"
			"The valid operands are C style operands : ==, !=, <, <=, >, >=, ||, &&.\n\n"
			"The special patterns are the following : \n"
			"- msg.timestamp : refers to the message timestamp\n"
			"- msg.len : refers to the message len\n"
			"- msg.type : refers to the message type\n"
			"- msg.id : refers to the message id\n"
			"- msg.data[0..7] : refers to the data byte of a message\n"
			"- msg.number : refers to the message number\n"
			"- contains(signal name) : to filter message that contains a certain signal\n"
			"- signal(name) : refers to the value of a signal\n\n"
			"Numbers can be specified as decimal, binary, hexadecimal or float (ex : 12, 0b100101, 0x12, 10.153)\n\n"
			"Usage examples :\n"
			"- \"msg.timestamp > 10 && contains(mySignal)\" : shows all message that have timestamp higher than 10s and that contains the signal \"mySignal\"\n"
			"- \"msg.data[1] != 0x02\" : shows all messages that have the second byte of data different of 2\n"
			"- \"signal(mySignal) > 2\" : shows all messages which contains the signal \"mySignal\" of value higher than 2\n"
			;
			//"- r(regex) : regular expression (not supported yet)\n"

	QMessageBox::information(this, "Filter help", help_text);
}

void CanTraceWidget::addGraph()
{
	// Find an unexisting name for the trace
	QString tabName;
	for(int i = 0; i < 100; i++)
	{
		tabName = "Graph" + QString::number(i);

		if(m_mainWindow->documentOf(tabName) == 0)
			break;
	}

	// Create document for the new tab
	Document *doc = new Document(CAN_GRAPH, tabName);
	connect(doc, SIGNAL(documentChanged()), m_mainWindow, SLOT(updateTabNames()));
	document()->addChildren(doc);

	// Create widget for the new tab
	GraphTab *tab = new GraphTab(m_dbc, this, doc, m_model);
	connect(this, SIGNAL(dbcChanged(DbcModel*)), tab, SLOT(setDbc(DbcModel*)));
	doc->setViewWidget(tab);

	// Add new tab to the MainWindow tab widget
	m_mainWindow->getTabWidget()->addTab(doc->getViewWidget(), doc->getIcon(), doc->fileName());
	m_mainWindow->getTabWidget()->setCurrentIndex(m_mainWindow->getTabWidget()->count() - 1);

	// Assign decoder and dbc to the graph widget
}
