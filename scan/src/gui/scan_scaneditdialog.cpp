/*****************************************************************
 * $Id: scan_scaneditdialog.cpp 1965 2013-08-08 16:33:15Z rutger $
 * Created: 2011-04-17 rutger
 *
 * Copyright (C) 2011 Red-Bag. All rights reserved.
 * This file is part of the Biluna SCAN project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#include "scan_scaneditdialog.h"

#include <QStringListModel>
#include "db_codehighlighter.h"
#include "scan_dialogfactory.h"
#include "scan_modelfactory.h"
#include "scan_preparejrresult.h"
#include "rb_datawidgetmapper.h"
#include "rb_mdiwindow.h"
#include "rb_sqlrelationaldelegate.h"
#include "rb_tedelegate.h"

/**
 * Constructor
 */
SCAN_ScanEditDialog::SCAN_ScanEditDialog(QWidget *parent)
                        : RB_Dialog(parent){

    setupUi(this);

    mScanModel = NULL;
    mQuestionModel = NULL;
    mAnswerModel = NULL;
    mBlockModel = NULL;
    mQuestionMapper = NULL;
    mAnswerMapper = NULL;
    mBlockMapper = NULL;

    mCodeHighlighter = new DB_CodeHighlighter(pteBlock->document());

    connect(teBlock, SIGNAL(contentsChanged()),
            this, SLOT(slotContentChanged()));
    connect(teBlock, SIGNAL(selectionChanged()),
            this, SLOT(adjustActions()));
    connect(pteBlock, SIGNAL(textChanged()),
            this, SLOT(slotContentChanged()));
    connect(twBlock, SIGNAL(currentChanged(int)),
            this, SLOT(changeTab(int)));

}

/**
 * Destructor
 */
SCAN_ScanEditDialog::~SCAN_ScanEditDialog() {
    // unique models are always deleted by the widgets,
    // can be MDI window or dialog, but also a dockWindow with table or tree
    delete mCodeHighlighter;

    delete mQuestionModel;
    delete mAnswerModel;
    delete mScanModel;
    delete mBlockModel;
    RB_DEBUG->print("SCAN_ScanEditDialog::~SCAN_ScanEditDialog() OK");
}

/**
 * Initialize widget and models
 */
void SCAN_ScanEditDialog::init() {
    setWindowTitle(getName());
//    teBlock->page()->setContentEditable(true);
    teBlock->installEventFilter(this);

    // Set model and connect to table view
    mScanModel = SCAN_MODELFACTORY->getModel(SCAN_ModelFactory::ModelScan);
    mScanModel->setRoot(SCAN_MODELFACTORY->getRootId());

    // Select rows
    mScanModel->select();

    // Set standard format and connection for model and view
    setFormatTableView(tvScan, mScanModel);

    // Hide columns
    for (int i = 0; i < mScanModel->columnCount(QModelIndex()); ++i) {
        if (i != RB2::HIDDENCOLUMNS) {
            tvScan->hideColumn(i);
        }
    }

    mQuestionModel = SCAN_MODELFACTORY->getModel(SCAN_ModelFactory::ModelQuestion);
    // mQuestionModel->setRoot("None"); // to create root

    // Mapper for line edits etc.
    mQuestionMapper = mQuestionModel->getMapper();
    mQuestionMapper->addMapping(leQuestionNumber, mQuestionModel->fieldIndex("number"));
    leQuestionNumber->setValidator(new QIntValidator(0, 9999, leQuestionNumber));
    RB_StringList items; // should correspond with SCAN2::ScanType
    items << tr("None") << tr("Multiple Choice Valued")
          << tr("Multiple Choice One Good") << tr("Open Questions");
    cbQuestionType->setModel(new QStringListModel(items, this));
    mQuestionMapper->addMapping(cbQuestionType, mQuestionModel->fieldIndex("type"),
                                "currentIndex");
    mQuestionMapper->addMapping(chbActive, mQuestionModel->fieldIndex("isactive"));
    mQuestionMapper->addMapping(teQuestionText, mQuestionModel->fieldIndex("question"));

    // Set standard format and connection for model and view
    setFormatTableView(tvQuestion, mQuestionModel);

    // Hide columns
    for (int i = 0; i < mQuestionModel->columnCount(QModelIndex()); ++i) {
        if (i != RB2::HIDDENCOLUMNS && i != RB2::HIDDENCOLUMNS + 3) {
            tvQuestion->hideColumn(i);
        }
    }

    mAnswerModel = SCAN_MODELFACTORY->getModel(SCAN_ModelFactory::ModelAnswer);
    // mAnswerModel->setRoot("None"); // to create root

    // Mapper for line edits etc.
    mAnswerMapper = mAnswerModel->getMapper();
    mAnswerMapper->addMapping(leAnswerNumber, mAnswerModel->fieldIndex("number"));
    leAnswerNumber->setValidator(new QIntValidator(0, 999, leAnswerNumber));
    mAnswerMapper->addMapping(teAnswerText, mAnswerModel->fieldIndex("answer"));

    // Set standard format and connection for model and view
    setFormatTableView(tvAnswer, mAnswerModel);

    // Hide columns
    for (int i = 0; i < mAnswerModel->columnCount(QModelIndex()); ++i) {
        if (i != RB2::HIDDENCOLUMNS && i != RB2::HIDDENCOLUMNS + 1) {
            tvAnswer->hideColumn(i);
        }
    }

    mBlockModel = SCAN_MODELFACTORY->getModel(SCAN_ModelFactory::ModelBlock);
    // mBlockModel->setRoot("None"); // to create root

    // Mapper for line edits etc.
    mBlockMapper = mBlockModel->getMapper();
    mBlockMapper->addMapping(leBlockNumber, mBlockModel->fieldIndex("number"));
    leBlockNumber->setValidator(new QIntValidator(0, 999, this));
    mBlockMapper->addMapping(pteBlock, mBlockModel->fieldIndex("blocktext"));

    SCAN_PrepareJrResult* oper = new SCAN_PrepareJrResult();
    cbContentType->setModel(new QStringListModel(oper->getContentTypeList(), this));
    mBlockMapper->addMapping(cbContentType, mBlockModel->fieldIndex("contenttype"),
                             "currentIndex");

    cbValidity1->setModel(new QStringListModel(oper->getValidityList(), this));
    mBlockMapper->addMapping(cbValidity1, mBlockModel->fieldIndex("var1type"),
                             "currentIndex");
    mBlockMapper->addMapping(leVal1Min, mBlockModel->fieldIndex("var1min"));
    leVal1Min->setValidator(new QIntValidator(0, 999, this));
    mBlockMapper->addMapping(leVal1Max, mBlockModel->fieldIndex("var1max"));
    leVal1Max->setValidator(new QIntValidator(0, 999, this));
    RB_StringList aoList;
    aoList << tr("None") << tr("And") << tr("Or");
    cbVal1AndOr->setModel(new QStringListModel(aoList, this));
    mBlockMapper->addMapping(cbVal1AndOr, mBlockModel->fieldIndex("andor1"),
                             "currentIndex");

    cbValidity2->setModel(new QStringListModel(oper->getValidityList(), this));
    mBlockMapper->addMapping(cbValidity2, mBlockModel->fieldIndex("var2type"),
                             "currentIndex");
    mBlockMapper->addMapping(leVal2Min, mBlockModel->fieldIndex("var2min"));
    leVal2Min->setValidator(new QIntValidator(0, 999, this));
    mBlockMapper->addMapping(leVal2Max, mBlockModel->fieldIndex("var2max"));
    leVal2Max->setValidator(new QIntValidator(0, 999, this));
    cbVal2AndOr->setModel(new QStringListModel(aoList, this));
    mBlockMapper->addMapping(cbVal2AndOr, mBlockModel->fieldIndex("andor2"),
                             "currentIndex");

    cbValidity3->setModel(new QStringListModel(oper->getValidityList(), this));
    mBlockMapper->addMapping(cbValidity3, mBlockModel->fieldIndex("var3type"),
                             "currentIndex");
    mBlockMapper->addMapping(leVal3Min, mBlockModel->fieldIndex("var3min"));
    leVal3Min->setValidator(new QIntValidator(0, 999, this));
    mBlockMapper->addMapping(leVal3Max, mBlockModel->fieldIndex("var3max"));
    leVal3Max->setValidator(new QIntValidator(0, 999, this));

    delete oper;

    // Set standard format and connection for model and view
    mBlockModel->select(); // only for childtree model once to get header cols
    setFormatTreeView(trvBlock, mBlockModel);
    trvBlock->setItemDelegateForColumn(mBlockModel->fieldIndex("blocktext"),
                                       new RB_TeDelegate(this));

    // Hide columns treeview
    for (int i = 0; i < mBlockModel->columnCount(QModelIndex()); ++i) {
        if (i != 0 && i != 1) {
            trvBlock->hideColumn(i);
        }
    }

    connect(trvBlock->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotUpdateWebView(QModelIndex,QModelIndex)));

    readSettings();
}

/**
 * Save data, can be called directly or indirectly via closeEvent()
 * and maybeSave().
 */
bool SCAN_ScanEditDialog::fileSave(bool /*withSelect*/) {
    if (!mScanModel || !mQuestionModel || !mAnswerModel) return false;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    mScanModel->submitAll();
    mQuestionModel->submitAll();
    mAnswerModel->submitAll();
    mBlockModel->submitAll();
    //  not required
    setWindowModified(false);

    QApplication::restoreOverrideCursor();

    return true;
}

/**
 * Reverse all changes
 */
void SCAN_ScanEditDialog::fileRevert() {
    if (!mScanModel || !mQuestionModel || !mAnswerModel || !mBlockModel) return;

    mScanModel->revert();
    mQuestionModel->revert();
    mAnswerModel->revert();
    mBlockModel->revert();
    setWindowModified(false);
}


/**
 * Button add (scan) clicked
 */
void SCAN_ScanEditDialog::on_pbAddScan_clicked() {
    if (!mScanModel) return;

    if (isWindowModified()) {
        fileSave(true);
    }


    // Clear sorting and filtering of proxyModel
    mScanModel->undoFilterSort();
    int row = 0; // mScanModel->rowCount(); // 0;
    mScanModel->insertRows(row, 1, QModelIndex());

    // NOTE: do not forget to set the default column values, specially for the
    //       relational table otherwise new row will not show!
    QModelIndex index = mScanModel->index(row, mScanModel->fieldIndex("title"));
    mScanModel->setData(index, "New", Qt::EditRole);

    tvScan->setCurrentIndex(mScanModel->index(row, RB2::HIDDENCOLUMNS, QModelIndex()));
    tvScan->scrollTo(mScanModel->index(row, RB2::HIDDENCOLUMNS, QModelIndex()));
    tabWidget->setCurrentIndex(0);
}

/**
 * Button delete (scan) clicked
 */
void SCAN_ScanEditDialog::on_pbDeleteScan_clicked() {
    if (!mScanModel) return;

    int res = SCAN_DIALOGFACTORY->requestYesNoDialog(tr("Delete item"),
                                          tr("Deletion cannot be undone.\n"
                                             "Do you want to delete the item?"));
    if (res != QMessageBox::Yes) return;

    QModelIndex index = tvScan->currentIndex();

    if (index.isValid()) {
        int row = index.row();
        mScanModel->removeRows(index.row(), 1, QModelIndex());
        fileSave(true);

        if (row > 0) {
            tvScan->setCurrentIndex(mScanModel->index(row-1, RB2::HIDDENCOLUMNS, QModelIndex()));
            tvScan->scrollTo(mScanModel->index(row-1, RB2::HIDDENCOLUMNS, QModelIndex()));
        }
    }
}

/**
 * Button save (scan) clicked
 */
void SCAN_ScanEditDialog::on_pbSaveScan_clicked() {
    fileSave();
}

/**
 * Button revert (scan) clicked
 */
void SCAN_ScanEditDialog::on_pbRevertScan_clicked() {
    fileRevert();
}

/**
 * Button add (question) clicked
 */
void SCAN_ScanEditDialog::on_pbAddQuestion_clicked() {
    if (!mQuestionModel) return;

    // Clear sorting and filtering of proxyModel
    mQuestionModel->undoFilterSort();

    QModelIndex index = tvScan->currentIndex();
    if (!index.isValid()) {
        SCAN_DIALOGFACTORY->requestWarningDialog(tr("No scan selected."));
        return;
    }

    int row = mQuestionModel->rowCount(); // 0;
    mQuestionModel->insertRows(row, 1, QModelIndex());

    // NOTE: do not forget to set the default column values, specially for the
    //       relational table otherwise new row will not show!
//    int rowCount = mQuestionModel->rowCountProxy(); // row is already added
    index = mQuestionModel->index(row, mQuestionModel->fieldIndex("number"));
    mQuestionModel->setData(index, row+1, Qt::EditRole);
    index = mQuestionModel->index(row, mQuestionModel->fieldIndex("type"));
    mQuestionModel->setData(index, 0, Qt::EditRole);
    index = mQuestionModel->index(row, mQuestionModel->fieldIndex("isactive"));
    mQuestionModel->setData(index, 1, Qt::EditRole);

    tvQuestion->setCurrentIndex(mQuestionModel->index(row, RB2::HIDDENCOLUMNS, QModelIndex()));
    tvQuestion->scrollTo(tvQuestion->currentIndex());
    leQuestionNumber->setFocus();
    leQuestionNumber->selectAll();
}

/**
 * Button delete (question) clicked
 */
void SCAN_ScanEditDialog::on_pbDeleteQuestion_clicked() {
    if (!mQuestionModel) return;

    int res = SCAN_DIALOGFACTORY->requestYesNoDialog(tr("Delete item"),
                                          tr("Deletion cannot be undone.\n"
                                             "Do you want to delete the item?"));
    if (res != QMessageBox::Yes) return;

    QModelIndex index = tvQuestion->currentIndex();

    if (index.isValid()) {
        int row = index.row();
        mQuestionModel->removeRows(index.row(), 1, QModelIndex());
        mQuestionModel->submitAllAndSelect();
        // setWindowModified(false); parent could have changed

        if (row > 0) {
            tvQuestion->setCurrentIndex(mQuestionModel->index(row-1, RB2::HIDDENCOLUMNS, QModelIndex()));
            tvQuestion->scrollTo(mQuestionModel->index(row-1, RB2::HIDDENCOLUMNS, QModelIndex()));
        }
    }
}

/**
 * Button add (answer) clicked
 */
void SCAN_ScanEditDialog::on_pbAddAnswer_clicked() {
    if (!mAnswerModel) return;

    // Clear sorting and filtering of proxyModel
    mAnswerModel->undoFilterSort();

    QModelIndex index = tvQuestion->currentIndex();
    if (!index.isValid()) {
        SCAN_DIALOGFACTORY->requestWarningDialog(tr("No question selected."));
        return;
    }

    int row = mAnswerModel->rowCount(); // 0;
    mAnswerModel->insertRows(row, 1, QModelIndex());

    // NOTE: do not forget to set the default column values, specially for the
    //       relational table otherwise new row will not show!
//    int rowCount = mAnswerModel->rowCountProxy();
    index = mAnswerModel->index(row, mAnswerModel->fieldIndex("number"));
    mAnswerModel->setData(index, row+1, Qt::EditRole);

    tvAnswer->setCurrentIndex(mAnswerModel->index(row, RB2::HIDDENCOLUMNS));
    tvAnswer->scrollTo(tvAnswer->currentIndex());
    leAnswerNumber->setFocus();
    leAnswerNumber->selectAll();
}

/**
 * Button delete (answer) clicked
 */
void SCAN_ScanEditDialog::on_pbDeleteAnswer_clicked() {
    if (!mAnswerModel) return;

    int res = SCAN_DIALOGFACTORY->requestYesNoDialog(tr("Delete item"),
                                          tr("Deletion cannot be undone.\n"
                                             "Do you want to delete the item?"));
    if (res != QMessageBox::Yes) return;

    QModelIndex index = tvAnswer->currentIndex();

    if (index.isValid()) {
        int row = index.row();
        mAnswerModel->removeRows(index.row(), 1, QModelIndex());
        mAnswerModel->submitAllAndSelect();
        // setWindowModified(false); parent could have changed


        if (row > 0) {
            tvAnswer->setCurrentIndex(mAnswerModel->index(row-1, RB2::HIDDENCOLUMNS, QModelIndex()));
            tvAnswer->scrollTo(mAnswerModel->index(row-1, RB2::HIDDENCOLUMNS, QModelIndex()));
        }
    }
}

/**
 * Button add block clicked
 */
void SCAN_ScanEditDialog::on_pbAddBlock_clicked() {
    // trvBlock
    if (!mBlockModel) return;

    mBlockModel->undoFilterSort();

    QModelIndex idxParent = QModelIndex();
    if (trvBlock->currentIndex().isValid()) {
        idxParent = trvBlock->currentIndex().parent();
    }

    int row = mBlockModel->rowCount(idxParent); // 0
    mBlockModel->insertRows(row, 1, idxParent);

    // NOTE: do not forget to set the default column values, specially for the
    //       relational table otherwise new row will not show!
    QModelIndex index = mBlockModel->index(row-1, mBlockModel->fieldIndex("number"), idxParent);
    int number = mBlockModel->data(index).toInt() + 1;
    index = mBlockModel->index(row, mBlockModel->fieldIndex("number"), idxParent);
    mBlockModel->setData(index, number, Qt::EditRole);
    // end NOTE

    trvBlock->setCurrentIndex(mBlockModel->index(row, RB2::HIDDENCOLUMNS, idxParent));
    trvBlock->scrollTo(mBlockModel->index(row, RB2::HIDDENCOLUMNS, idxParent));
    leBlockNumber->setFocus();
    leBlockNumber->selectAll();
}

/**
 * Button add subblock clicked
 */
void SCAN_ScanEditDialog::on_pbAddSubBlock_clicked() {
    if (!mBlockModel) return;

    QModelIndex idxParent = QModelIndex();
    if (trvBlock->currentIndex().isValid()) {
        idxParent = trvBlock->currentIndex();
    }
    int row = mBlockModel->rowCount(idxParent);
    mBlockModel->insertRows(row, 1, idxParent);

    // NOTE: do not forget to set the default column values, specially for the
    //       relational table (in tableView otherwise new row will not show)
    QModelIndex index = mBlockModel->index(row-1, mBlockModel->fieldIndex("number"), idxParent);
    int number = mBlockModel->data(index).toInt() + 1;
    index = mBlockModel->index(row, mBlockModel->fieldIndex("number"), idxParent);
    mBlockModel->setData(index, number, Qt::EditRole);
    // end NOTE

    trvBlock->setCurrentIndex(mBlockModel->index(row, 0, idxParent));
    trvBlock->scrollTo(mBlockModel->index(row, RB2::HIDDENCOLUMNS, idxParent));
    leBlockNumber->setFocus();
    leBlockNumber->selectAll();
}

/**
 * Button delete block clicked
 */
void SCAN_ScanEditDialog::on_pbDeleteBlock_clicked() {
    if (!mBlockModel) return;

    int res = SCAN_DIALOGFACTORY->requestYesNoDialog(tr("Delete item"),
                                          tr("Deletion cannot be undone.\n"
                                             "Do you want to delete the item?"));
    if (res != QMessageBox::Yes) return;

    QModelIndex index = trvBlock->currentIndex();
    if (index.isValid()) {
        mBlockModel->removeRows(index.row(), 1, index.parent());
        mBlockModel->submitAllAndSelect();
        // setWindowModified(false); parent could have changed
    }
}

/**
 * Button promote block clicked
 */
void SCAN_ScanEditDialog::on_pbPromoteBlock_clicked() {
    promote(trvBlock);
}

/**
 * Button demote block clicked
 */
void SCAN_ScanEditDialog::on_pbDemoteBlock_clicked() {
    demote(trvBlock);
}

/**
 * Button align left clicked
 */
void SCAN_ScanEditDialog::on_pbAlignLeft_clicked() {
//    teBlock->triggerPageAction(QWebPage::AlignLeft);
}

/**
 * Button align right clicked
 */
void SCAN_ScanEditDialog::on_pbAlignRight_clicked() {
//    teBlock->triggerPageAction(QWebPage::AlignRight);
}

/**
 * Button align center clicked
 */
void SCAN_ScanEditDialog::on_pbAlignCenter_clicked() {
//    teBlock->triggerPageAction(QWebPage::AlignCenter);
}

/**
 * Button align justify clicked
 */
void SCAN_ScanEditDialog::on_pbAlignJustify_clicked() {
//    teBlock->triggerPageAction(QWebPage::AlignJustified);
}

/**
 * Button bold clicked
 */
void SCAN_ScanEditDialog::on_pbBold_clicked() {
//    teBlock->triggerPageAction(QWebPage::ToggleBold);
}

/**
 * Button italic clicked
 */
void SCAN_ScanEditDialog::on_pbItalic_clicked() {
//    teBlock->triggerPageAction(QWebPage::ToggleItalic);
}

/**
 * Button underline clicked
 */
void SCAN_ScanEditDialog::on_pbUnderline_clicked() {
//    teBlock->triggerPageAction(QWebPage::ToggleUnderline);
}

/**
 * Button strikethrough clicked
 */
void SCAN_ScanEditDialog::on_pbStrikethrough_clicked() {
//    teBlock->triggerPageAction(QWebPage::ToggleStrikethrough);
}

/**
 * Button increase indent clicked
 */
void SCAN_ScanEditDialog::on_pbIndent_clicked() {
//    teBlock->triggerPageAction(QWebPage::Indent);
}

/**
 * Button decrease indent clicked
 */
void SCAN_ScanEditDialog::on_pbOutdent_clicked() {
//    teBlock->triggerPageAction(QWebPage::Outdent);
}

/**
 * Button numbered list clicked
 */
void SCAN_ScanEditDialog::on_pbNumberList_clicked() {
//    teBlock->triggerPageAction(QWebPage::InsertOrderedList);
}

/**
 * Button bulleted list clicked
 */
void SCAN_ScanEditDialog::on_pbBulletList_clicked() {
//    teBlock->triggerPageAction(QWebPage::InsertUnorderedList);
}

/**
 * Button font name clicked
 */
void SCAN_ScanEditDialog::on_pbFontName_clicked() {
    QStringList families = QFontDatabase().families();
    bool ok = false;
    QString family = QInputDialog::getItem(this, tr("Font"), tr("Select font:"),
                                           families, 0, false, &ok);

    if (ok)
        execCommand("fontName", family);
}

/**
 * Button font size clicked
 */
void SCAN_ScanEditDialog::on_pbFontSize_clicked() {
    QStringList sizes;
    sizes << "h1";
    sizes << "h2";
    sizes << "h3";
    sizes << "h4";
    sizes << "xx-small";
    sizes << "x-small";
    sizes << "small";
    sizes << "medium";
    sizes << "large";
    sizes << "x-large";
    sizes << "xx-large";

    bool ok = false;
    QString size = QInputDialog::getItem(this, tr("Font Size"), tr("Select font size:"),
                                        sizes, sizes.indexOf("medium"), false, &ok);

    if (ok) {
        if (sizes.indexOf(size) < 4) {
            execCommand("formatBlock", size);
        } else {
            execCommand("fontSize", QString::number(sizes.indexOf(size)));
        }
    }
}

/**
 * Button text color clicked
 */
void SCAN_ScanEditDialog::on_pbColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::black, this);
    if (color.isValid())
        execCommand("foreColor", color.name());
}

/**
 * Button text background color clicked
 */
void SCAN_ScanEditDialog::on_pbBgColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid())
        execCommand("hiliteColor", color.name());
}

/**
 * Execute command using the Javascript engine, HACK for functions not yet
 * directly available in API
 * @param cmd command string
 */
void SCAN_ScanEditDialog::execCommand(const QString &cmd) {
//    QWebFrame* frame = teBlock->page()->mainFrame();
//    QString js = QString("document.execCommand(\"%1\", false, null)").arg(cmd);
//    frame->evaluateJavaScript(js);
}

/**
 * Execute command using the Javascript engine, HACK for functions not yet
 * directly available in API
 * @param cmd command string
 * @param arg additional arguments
 */
void SCAN_ScanEditDialog::execCommand(const QString &cmd, const QString &arg) {
//    QWebFrame* frame = teBlock->page()->mainFrame();
//    QString js = QString("document.execCommand(\"%1\", false, \"%2\")").arg(cmd).arg(arg);
//    frame->evaluateJavaScript(js);
}

/**
 * Determines the current state of the named command using the Javascript engine,
 * HACK for functions not yet directly available in API, strikeThrough,
 * orderedList and unorderdList
 * @param cmd command
 * @return true
 */
bool SCAN_ScanEditDialog::queryCommandState(const QString &cmd) {
//    QWebFrame* frame = teBlock->page()->mainFrame();
//    QString js = QString("document.queryCommandState(\"%1\", false, null)").arg(cmd);
//    QVariant result = frame->evaluateJavaScript(js);
//    return result.toString().simplified().toLower() == "true";
    return false;
}

/**
 * Determines the current checked state of the enumerated action
 * @param wa enumerator of QWebPage::WebAction such as QWebPage::ToggleBold
 * @return true if valid
 */
//bool SCAN_ScanEditDialog::pageActionChecked(QWebPage::WebAction wa) {
//    return teBlock->pageAction(wa)->isChecked();
//}

/**
 * Determines the current enabled state of the enumerated action
 * @param wa enumerator of QWebPage::WebAction such as QWebPage::Undo
 * @return true if valid
 */
//bool SCAN_ScanEditDialog::pageActionEnabled(QWebPage::WebAction wa) {
//    return teBlock->pageAction(wa)->isEnabled();
//}

/**
 * Adjust buttons for text editing based on text selection
 */
void SCAN_ScanEditDialog::adjustActions() {
    // TODO: replace teBlock by RB_TextEdit
//    pbBold->setChecked(pageActionChecked(QWebPage::ToggleBold));
//    pbItalic->setChecked(pageActionChecked(QWebPage::ToggleItalic));
//    pbUnderline->setChecked(pageActionChecked(QWebPage::ToggleUnderline));
//    pbStrikethrough->setChecked(queryCommandState("strikeThrough"));
//    pbNumberList->setChecked(queryCommandState("insertOrderedList"));
//    pbBulletList->setChecked(queryCommandState("insertUnorderedList"));
}

/**
 * Change tab
 */
void SCAN_ScanEditDialog::changeTab(int index) {
    if (mSourceDirty && (index == 1)) {
        RB_String content = teBlock->toHtml();
        pteBlock->setPlainText(content);
        mSourceDirty = false;
    } else if (mSourceDirty && (index == 0)) {
        RB_String content = pteBlock->toPlainText();
        teBlock->setHtml(content);
        mSourceDirty = false;
    }
}

/**
 * Slot for content of webview and textedit for block has changed
 */
void SCAN_ScanEditDialog::slotContentChanged() {
    if (teBlock->hasFocus() || pteBlock->hasFocus()) {
        mSourceDirty = true;
        setWindowModified(true);
    }
}

/**
 * Slot to update webview when textedit for textblock has changed
 */
void SCAN_ScanEditDialog::slotUpdateWebView(const QModelIndex& /*current*/,
                                            const QModelIndex& /*previous*/) {
    RB_String content = mBlockModel->getCurrentValue("blocktext").toString();
    teBlock->setHtml(content);
}

/**
 * Button Help clicked
 */
void SCAN_ScanEditDialog::on_pbHelp_clicked() {
    SCAN_DIALOGFACTORY->getMainWindow()->slotHelpSubject(objectName());
}

/**
 * Button OK clicked
 */
void SCAN_ScanEditDialog::on_pbOk_clicked() {
    fileSave();
    accept();
}

/**
 * Button Cancel clicked
 */
void SCAN_ScanEditDialog::on_pbCancel_clicked() {
    reject();
}

/**
 * Change event, for example translation
 */
void SCAN_ScanEditDialog::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
 * Event filter handling the QWebEngineView losing focus
 * and updating the QTextEdit which is mapped to model
 * @param obj QWebEngineView with marked-up block text
 * @param e QEvent event
 */
bool SCAN_ScanEditDialog::eventFilter(QObject* obj, QEvent* e) {
//    if (teBlock == obj && e->type() == QEvent::FocusOut) {
//        if (teBlock->isWindowModified()) {
//            RB_String content = teBlock->page()->mainFrame()->toHtml();
//            mBlockModel->setCurrentValue("blocktext", content, Qt::EditRole);
//            mSourceDirty = false;
//        }
//    }

    return RB_Dialog::eventFilter(obj, e);
}

