﻿#include "platform.h"

#include "main_wnd.h"
#include "resource.h"

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("الكتاب المقدس");

WNDPROC lpTabControlWndProc;
WNDPROC lpTextEditWndProc;

static LRESULT HandleMessage(BaseWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT TabControlCallBckProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT TextEditCallBckProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static int g_statusbar_height;
static const int g_margin = 5;

void OnDBClick_treeView(MainWindow* mw, WPARAM wParam, LPARAM lParam);
void OnBtnClicked_next_chapter(MainWindow* mw, WPARAM wParam, LPARAM lParam);
void OnBtnClicked_prev_chapter(MainWindow* mw, WPARAM wParam, LPARAM lParam);
void OnBtnClicked_search(MainWindow* mw, WPARAM wParam, LPARAM lParam);
void OnTXChaper_enter(MainWindow* mw, WPARAM wParam, LPARAM lParam);

ATOM MainWindow_RegisterClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = BaseWindow_Proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCE(IDR_POPUPMENU);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PESHITTA));

    return RegisterClassEx(&wcex);
}

BOOL Create(BaseWindow* _this)
{
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        _this
    );

    _this->_hWnd = hWnd;

    return _this->_hWnd ? TRUE : FALSE;
}

MainWindow* MainWindow_init()
{
    MainWindow* mw = (MainWindow*)malloc(sizeof(MainWindow));
    assert(mw != NULL);

    BaseWindow_default((BaseWindow*)mw);

    mw->_client_width = mw->_client_height = 0;

    mw->_baseWindow._HandleMessageFunc = HandleMessage;
    mw->_baseWindow._CreateFunc = Create;

    mw->_lm_main = LayoutManager_init();
    mw->_lm_tab_bible = LayoutManager_init();
    mw->_lm_tab_bible_bottom = LayoutManager_init();
    mw->_lm_tab_search = LayoutManager_init();
    mw->_lm_tab_bible_sy = LayoutManager_init();

    mw->_statusBar = StatusBar_init();
    mw->_treeView = TreeView_init();
    mw->_tabControl = TabControl_init();
    mw->_richEdit = RichEdit_init();
    mw->_richEdit_sy = RichEdit_init();
    mw->_lb_chapter = Label_init();
    mw->_tx_chapter_idx = TextEdit_init();
    mw->_lb_chapter_count = Label_init();
    mw->_tx_search = TextEdit_init();
    mw->_lv_result = ListView_init();
    mw->_bt_next_chapter = Button_init();
    mw->_bt_prev_chapter = Button_init();
    mw->_bt_search = Button_init();

    return mw;
}

void MainWindow_free(MainWindow* mw)
{
    Button_free(mw->_bt_search);
    Button_free(mw->_bt_prev_chapter);
    Button_free(mw->_bt_next_chapter);
    ListView_free(mw->_lv_result);
    TextEdit_free(mw->_tx_search);
    Label_free(mw->_lb_chapter_count);
    TextEdit_free(mw->_tx_chapter_idx);
    Label_free(mw->_lb_chapter);
    RichEdit_free(mw->_richEdit_sy);
    RichEdit_free(mw->_richEdit);
    TabControl_free(mw->_tabControl);
    TreeView_free(mw->_treeView);
    StatusBar_free(mw->_statusBar);

    LayoutManager_free(mw->_lm_tab_bible_sy); 
    LayoutManager_free(mw->_lm_tab_search);
    LayoutManager_free(mw->_lm_tab_bible_bottom);
    LayoutManager_free(mw->_lm_tab_bible);
    LayoutManager_free(mw->_lm_main);

    free(mw);
}

static void OnCreate_TreeView(MainWindow* mw)
{
    mw->_treeView->_baseWindow._SetParentFunc((BaseWindow*)mw->_treeView, mw->_baseWindow._hWnd);
    mw->_treeView->_baseWindow._SetIdFunc((BaseWindow*)mw->_treeView, (HMENU)ID_TREEVIEW);

    if (!mw->_treeView->_baseWindow._CreateFunc((BaseWindow*)mw->_treeView))
    {
        ShowError(L"Unable to create tree view!");
        return;
    }

    HTREEITEM hItem;
    TVINSERTSTRUCT insertStruct = { 0 };
    TVITEM* pItem = &insertStruct.item;
    insertStruct.hParent = NULL;
    insertStruct.hInsertAfter = TVI_ROOT;

    pItem->mask = TVIF_TEXT;
    pItem->pszText = L"العهد القديم";
    hItem = (HTREEITEM)SendMessageA(mw->_treeView->_baseWindow._hWnd, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);

    if (hItem)
    {
        HTREEITEM hItem_sub;

        for (int ix = 0; ix < sizeof(OldTestament) / sizeof(OldTestament[0]); ++ix)
        {
            insertStruct.hParent = hItem;
            pItem->pszText = OldTestament[ix].table_arabic;
            hItem_sub = (HTREEITEM)SendMessageA(mw->_treeView->_baseWindow._hWnd, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);
            if (hItem_sub) SendMessage(mw->_treeView->_baseWindow._hWnd, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem_sub);
        }
    }

    insertStruct.hParent = NULL;

    pItem->mask = TVIF_TEXT;
    pItem->pszText = L"العهد الجديد";
    hItem = (HTREEITEM)SendMessageA(mw->_treeView->_baseWindow._hWnd, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);

    if (hItem)
    {
        HTREEITEM hItem_sub;

        for (int ix = 0; ix < sizeof(NewTestament) / sizeof(NewTestament[0]); ++ix)
        {
            insertStruct.hParent = hItem;
            pItem->pszText = NewTestament[ix].table_arabic;
            hItem_sub = (HTREEITEM)SendMessageA(mw->_treeView->_baseWindow._hWnd, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);
            if (hItem_sub) SendMessage(mw->_treeView->_baseWindow._hWnd, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem_sub);
        }
    }
}

static void OnCreate_TabControl(MainWindow* mw)
{
    mw->_tabControl->_baseWindow._SetParentFunc((BaseWindow*)mw->_tabControl, mw->_baseWindow._hWnd);
    mw->_tabControl->_baseWindow._SetIdFunc((BaseWindow*)mw->_tabControl, (HMENU)ID_TABCONTROL);

    if (!mw->_tabControl->_baseWindow._CreateFunc((BaseWindow*)mw->_tabControl))
    {
        ShowError(L"Unable to create tab control!");
        return;
    }

    TCITEM tie;

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;
    tie.pszText = L"الانجيل";
    TabCtrl_InsertItem(mw->_tabControl->_baseWindow._hWnd, 0, &tie);
    
    tie.mask = TCIF_TEXT;
    tie.iImage = -1;
    tie.pszText = L"البحث";
    TabCtrl_InsertItem(mw->_tabControl->_baseWindow._hWnd, 1, &tie);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;
    tie.pszText = L"ܦܫܺܝܛܬܳܐ";
    TabCtrl_InsertItem(mw->_tabControl->_baseWindow._hWnd, 2, &tie);
    
    
    mw->_richEdit->_baseWindow._SetParentFunc((BaseWindow*)mw->_richEdit, mw->_tabControl->_baseWindow._hWnd);
    mw->_richEdit->_baseWindow._SetIdFunc((BaseWindow*)mw->_richEdit, (HMENU)ID_RICHEDIT);

    if (!mw->_richEdit->_baseWindow._CreateFunc((BaseWindow*)mw->_richEdit))
    {
        ShowError(L"Unable to create rich edit!");
        return;
    }

    mw->_richEdit_sy->_baseWindow._SetParentFunc((BaseWindow*)mw->_richEdit_sy, mw->_tabControl->_baseWindow._hWnd);
    mw->_richEdit_sy->_baseWindow._SetIdFunc((BaseWindow*)mw->_richEdit_sy, (HMENU)ID_RICHEDIT_SY);

    if (!mw->_richEdit_sy->_baseWindow._CreateFunc((BaseWindow*)mw->_richEdit_sy))
    {
        ShowError(L"Unable to create rich sy edit!");
        return;
    }

    mw->_lb_chapter->_baseWindow._SetParentFunc((BaseWindow*)mw->_lb_chapter, mw->_tabControl->_baseWindow._hWnd);
    mw->_lb_chapter->_baseWindow._SetIdFunc((BaseWindow*)mw->_lb_chapter, (HMENU)ID_LABELCHAPTER);

    if (!mw->_lb_chapter->_baseWindow._CreateFunc((BaseWindow*)mw->_lb_chapter))
    {
        ShowError(L"Unable to create label chapter!");
        return;
    }

#ifdef _WIN64
    lpTabControlWndProc = (WNDPROC)SetWindowLongPtr(mw->_tabControl->_baseWindow._hWnd,
        GWLP_WNDPROC, (LONG_PTR)&TabControlCallBckProcedure);
    SetWindowLongPtr(mw->_tabControl->_baseWindow._hWnd,
        GWLP_USERDATA, (LONG_PTR)mw);
#else
    lpTabControlWndProc = (WNDPROC)SetWindowLongPtr(mw->_tabControl->_baseWindow._hWnd, 
        GWL_WNDPROC, (LONG_PTR)&TabControlCallBckProcedure);
    SetWindowLongPtr(mw->_tabControl->_baseWindow._hWnd,
        GWL_USERDATA, (LONG_PTR)mw);
#endif



    SetWindowText(mw->_lb_chapter->_baseWindow._hWnd, L"الاصحاح");

    mw->_tx_chapter_idx->_baseWindow._SetParentFunc((BaseWindow*)mw->_tx_chapter_idx, mw->_tabControl->_baseWindow._hWnd);
    mw->_tx_chapter_idx->_baseWindow._SetIdFunc((BaseWindow*)mw->_tx_chapter_idx, (HMENU)ID_LABELCHAPTER_COUNT2);

    if (!mw->_tx_chapter_idx->_baseWindow._CreateFunc((BaseWindow*)mw->_tx_chapter_idx))
    {
        ShowError(L"Unable to create chapter count 1!");
        return;
    }

#ifdef _WIN64
    lpTextEditWndProc = (WNDPROC)SetWindowLongPtr(mw->_tx_chapter_idx->_baseWindow._hWnd,
        GWLP_WNDPROC, (LONG_PTR)&TextEditCallBckProcedure);
    SetWindowLongPtr(mw->_tx_chapter_idx->_baseWindow._hWnd,
        GWLP_USERDATA, (LONG_PTR)mw);
#else
    lpTextEditWndProc = (WNDPROC)SetWindowLongPtr(mw->_tx_chapter_idx->_baseWindow._hWnd,
        GWL_WNDPROC, (LONG_PTR)&TextEditCallBckProcedure);
    SetWindowLongPtr(mw->_tx_chapter_idx->_baseWindow._hWnd,
        GWL_USERDATA, (LONG_PTR)mw);
#endif

    mw->_lb_chapter_count->_baseWindow._SetParentFunc((BaseWindow*)mw->_lb_chapter_count, mw->_tabControl->_baseWindow._hWnd);
    mw->_lb_chapter_count->_baseWindow._SetIdFunc((BaseWindow*)mw->_lb_chapter_count, (HMENU)ID_LABELCHAPTER_COUNT2);

    if (!mw->_lb_chapter_count->_baseWindow._CreateFunc((BaseWindow*)mw->_lb_chapter_count))
    {
        ShowError(L"Unable to create chapter count 2!");
        return;
    }

    mw->_bt_prev_chapter->_baseWindow._SetParentFunc((BaseWindow*)mw->_bt_prev_chapter, mw->_tabControl->_baseWindow._hWnd);
    mw->_bt_prev_chapter->_baseWindow._SetIdFunc((BaseWindow*)mw->_bt_prev_chapter, (HMENU)ID_BUTTON_PREV_CHAPTER);

    if (!mw->_bt_prev_chapter->_baseWindow._CreateFunc((BaseWindow*)mw->_bt_prev_chapter))
    {
        ShowError(L"Unable to create prev chapter button");
        return;
    }

    SetWindowText(mw->_bt_prev_chapter->_baseWindow._hWnd, L"السابق");

    mw->_bt_next_chapter->_baseWindow._SetParentFunc((BaseWindow*)mw->_bt_next_chapter, mw->_tabControl->_baseWindow._hWnd);
    mw->_bt_next_chapter->_baseWindow._SetIdFunc((BaseWindow*)mw->_bt_next_chapter, (HMENU)ID_BUTTON_NEXT_CHAPTER);

    if (!mw->_bt_next_chapter->_baseWindow._CreateFunc((BaseWindow*)mw->_bt_next_chapter))
    {
        ShowError(L"Unable to create next chapter button");
        return;
    }

    SetWindowText(mw->_bt_next_chapter->_baseWindow._hWnd, L"التالي");

    mw->_tx_search->_baseWindow._SetParentFunc((BaseWindow*)mw->_tx_search, mw->_tabControl->_baseWindow._hWnd);
    mw->_tx_search->_baseWindow._SetIdFunc((BaseWindow*)mw->_tx_search, (HMENU)ID_TEXTEDIT_SEARCH);

    if (!mw->_tx_search->_baseWindow._CreateFunc((BaseWindow*)mw->_tx_search))
    {
        ShowError(L"Unable to create search text edit!");
        return;
    }

    mw->_bt_search->_baseWindow._SetParentFunc((BaseWindow*)mw->_bt_search, mw->_tabControl->_baseWindow._hWnd);
    mw->_bt_search->_baseWindow._SetIdFunc((BaseWindow*)mw->_bt_search, (HMENU)ID_BUTTON_SEARCH);

    if (!mw->_bt_search->_baseWindow._CreateFunc((BaseWindow*)mw->_bt_search))
    {
        ShowError(L"Unable to create search button");
        return;
    }

    SetWindowText(mw->_bt_search->_baseWindow._hWnd, L"بحث");

    mw->_lv_result->_baseWindow._SetParentFunc((BaseWindow*)mw->_lv_result, mw->_tabControl->_baseWindow._hWnd);
    mw->_lv_result->_baseWindow._SetIdFunc((BaseWindow*)mw->_lv_result, (HMENU)ID_LISTVIW_RESULT);

    if (!mw->_lv_result->_baseWindow._CreateFunc((BaseWindow*)mw->_lv_result))
    {
        ShowError(L"Unable to create search list view!");
        return;
    }

    LV_COLUMN lvC;
    lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvC.fmt = LVCFMT_LEFT;
    lvC.cx = 100;

    lvC.pszText = L"مكان";
    ListView_InsertColumn(mw->_lv_result->_baseWindow._hWnd, 0, &lvC);
    
    lvC.cx = 300; 
    lvC.pszText = L"النص";
    ListView_InsertColumn(mw->_lv_result->_baseWindow._hWnd, 1, &lvC);

    ShowWindow(mw->_tx_search->_baseWindow._hWnd, SW_HIDE);
    ShowWindow(mw->_bt_search->_baseWindow._hWnd, SW_HIDE);
    ShowWindow(mw->_lv_result->_baseWindow._hWnd, SW_HIDE);
}

static void OnCreate(MainWindow* mw)
{
    RECT rc;
    GetClientRect(mw->_baseWindow._hWnd, &rc);

    mw->_client_width = rc.right - rc.left;
    mw->_client_height = rc.bottom - rc.top;

    mw->_statusBar->_baseWindow._SetParentFunc((BaseWindow*)mw->_statusBar, mw->_baseWindow._hWnd);
    mw->_statusBar->_baseWindow._SetIdFunc((BaseWindow*)mw->_statusBar, (HMENU)ID_STATUSBAR);

    if (!mw->_statusBar->_baseWindow._CreateFunc((BaseWindow*)mw->_statusBar))
    {
        ShowError(L"Unable to create status bar!");
        return;
    }

    RECT sBarRc;
    GetWindowRect(mw->_statusBar->_baseWindow._hWnd, &sBarRc);
    g_statusbar_height = sBarRc.bottom - sBarRc.top + 1;

    mw->_lm_main->_InitFunc(mw->_lm_main, 1, 2);
    mw->_lm_main->_SetRowsHeightFunc(mw->_lm_main, 1.0);
    mw->_lm_main->_SetColumnWidthFunc(mw->_lm_main, 0.2, 0.8);

    Margin m1 = { 2, 2, 5, 2 } , m2 = { 5, 2, 2, 2 };
    mw->_lm_main->_SetCmpFunc(mw->_lm_main, 0, 0, (Component*)mw->_treeView, LM_H_EXPAND | LM_V_EXPAND, m1);
    mw->_lm_main->_SetCmpFunc(mw->_lm_main, 0, 1, (Component*)mw->_tabControl, LM_H_EXPAND | LM_V_EXPAND, m2);

    mw->_lm_tab_bible->_InitFunc(mw->_lm_tab_bible, 2, 1);
    mw->_lm_tab_bible->_SetRowsHeightFunc(mw->_lm_tab_bible, 1.0, 37.0);
    mw->_lm_tab_bible->_SetColumnWidthFunc(mw->_lm_tab_bible, 1.0);

    Margin m3 = { 5, 5, 5, 0 };
    mw->_lm_tab_bible->_SetCmpFunc(mw->_lm_tab_bible, 0, 0, (Component*)mw->_richEdit, LM_H_EXPAND | LM_V_EXPAND, m3);
    mw->_lm_tab_bible->_SetCmpFunc(mw->_lm_tab_bible, 1, 0, (Component*)mw->_lm_tab_bible_bottom, LM_H_EXPAND | LM_V_EXPAND, m3);

    mw->_lm_tab_bible_bottom->_InitFunc(mw->_lm_tab_bible_bottom, 1, 5);
    mw->_lm_tab_bible_bottom->_SetRowsHeightFunc(mw->_lm_tab_bible_bottom, 1.0);
    mw->_lm_tab_bible_bottom->_SetColumnWidthFunc(mw->_lm_tab_bible_bottom, 100.0, 70.0, 70.0, 90.0, 90.0);

    Margin m4 = { 5, 5, 5, 5 };
    mw->_lm_tab_bible_bottom->_SetCmpFunc(mw->_lm_tab_bible_bottom, 0, 0, (Component*)mw->_lb_chapter, LM_H_EXPAND | LM_V_EXPAND, m4);
    mw->_lm_tab_bible_bottom->_SetCmpFunc(mw->_lm_tab_bible_bottom, 0, 1, (Component*)mw->_tx_chapter_idx, LM_H_EXPAND | LM_V_EXPAND, m4);
    mw->_lm_tab_bible_bottom->_SetCmpFunc(mw->_lm_tab_bible_bottom, 0, 2, (Component*)mw->_lb_chapter_count, LM_H_EXPAND | LM_V_EXPAND, m4);
    mw->_lm_tab_bible_bottom->_SetCmpFunc(mw->_lm_tab_bible_bottom, 0, 3, (Component*)mw->_bt_prev_chapter, LM_H_EXPAND | LM_V_EXPAND, m4);
    mw->_lm_tab_bible_bottom->_SetCmpFunc(mw->_lm_tab_bible_bottom, 0, 4, (Component*)mw->_bt_next_chapter, LM_H_EXPAND | LM_V_EXPAND, m4);
    
    mw->_lm_tab_search->_InitFunc(mw->_lm_tab_search, 3, 1);
    mw->_lm_tab_search->_SetRowsHeightFunc(mw->_lm_tab_search, 40.0, 40.0, 1.0);
    mw->_lm_tab_search->_SetColumnWidthFunc(mw->_lm_tab_search, 1.0);

    Margin m5 = { 5, 5, 5, 5 };
    mw->_lm_tab_search->_SetCmpFunc(mw->_lm_tab_search, 0, 0, (Component*)mw->_tx_search, LM_H_EXPAND | LM_V_EXPAND, m5);
    mw->_lm_tab_search->_SetCmpFunc(mw->_lm_tab_search, 0, 1, (Component*)mw->_bt_search, LM_H_EXPAND | LM_V_EXPAND, m5);
    mw->_lm_tab_search->_SetCmpFunc(mw->_lm_tab_search, 0, 2, (Component*)mw->_lv_result, LM_H_EXPAND | LM_V_EXPAND, m5);

    mw->_lm_tab_bible_sy->_InitFunc(mw->_lm_tab_bible_sy, 2, 1);
    mw->_lm_tab_bible_sy->_SetRowsHeightFunc(mw->_lm_tab_bible_sy, 1.0, 37.0);
    mw->_lm_tab_bible_sy->_SetColumnWidthFunc(mw->_lm_tab_bible_sy, 1.0);

    Margin m6 = { 5, 5, 5, 0 };
    mw->_lm_tab_bible_sy->_SetCmpFunc(mw->_lm_tab_bible_sy, 0, 0, (Component*)mw->_richEdit_sy, LM_H_EXPAND | LM_V_EXPAND, m6);
    mw->_lm_tab_bible_sy->_SetCmpFunc(mw->_lm_tab_bible, 1, 0, (Component*)mw->_lm_tab_bible_bottom, LM_H_EXPAND | LM_V_EXPAND, m6);

    OnCreate_TreeView(mw);
    OnCreate_TabControl(mw);

    LoadPart(mw, NewTestament[0].table_english);
    LoadChapter(mw, "matthew", 1);
    LoadChapter_sy(mw, "matthew_sy", 1);
}

static void OnSize(MainWindow* mw, int width, int height)
{
    mw->_client_width = width;
    mw->_client_height = height;

    MoveWindow(mw->_statusBar->_baseWindow._hWnd, 0, height - g_statusbar_height, width, g_statusbar_height, TRUE);

    mw->_lm_main->_DoLayoutFunc(mw->_lm_main, 0, 0, width, height - g_statusbar_height, TRUE, NULL);

    RECT rc;
    TabCtrl_GetItemRect(mw->_tabControl->_baseWindow._hWnd, 0, &rc);

    int iHeight = rc.bottom - rc.top;
    GetClientRect(mw->_tabControl->_baseWindow._hWnd, &rc);

    mw->_lm_tab_bible->_DoLayoutFunc(mw->_lm_tab_bible, rc.left, rc.top + iHeight, 
        rc.right - rc.left, rc.bottom - rc.top - iHeight, TRUE, NULL);

    mw->_lm_tab_search->_DoLayoutFunc(mw->_lm_tab_search, rc.left, rc.top + iHeight,
        rc.right - rc.left, rc.bottom - rc.top - iHeight, TRUE, NULL);

    mw->_lm_tab_bible_sy->_DoLayoutFunc(mw->_lm_tab_bible_sy, rc.left, rc.top + iHeight,
        rc.right - rc.left, rc.bottom - rc.top - iHeight, TRUE, NULL);
}

static void OnNotify_tabControl(MainWindow* mw, WPARAM wParam, LPARAM lParam)
{
    switch (((LPNMHDR)lParam)->code)
    {
    case TCN_SELCHANGE:
    {
        int iPage = TabCtrl_GetCurSel(mw->_tabControl->_baseWindow._hWnd);
        if (iPage == 0)
        {

            ShowWindow(mw->_richEdit->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_lb_chapter->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_tx_chapter_idx->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_lb_chapter_count->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_bt_next_chapter->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_bt_prev_chapter->_baseWindow._hWnd, SW_SHOW);

            ShowWindow(mw->_tx_search->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_bt_search->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_lv_result->_baseWindow._hWnd, SW_HIDE);

            ShowWindow(mw->_richEdit_sy->_baseWindow._hWnd, SW_HIDE);
        }
        else if (iPage == 1)
        {
            ShowWindow(mw->_richEdit->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_lb_chapter->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_tx_chapter_idx->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_lb_chapter_count->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_bt_next_chapter->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_bt_prev_chapter->_baseWindow._hWnd, SW_HIDE);

            ShowWindow(mw->_tx_search->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_bt_search->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_lv_result->_baseWindow._hWnd, SW_SHOW);

            ShowWindow(mw->_richEdit_sy->_baseWindow._hWnd, SW_HIDE);
        }
        else if (iPage == 2)
        {
            ShowWindow(mw->_richEdit->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_lb_chapter->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_tx_chapter_idx->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_lb_chapter_count->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_bt_next_chapter->_baseWindow._hWnd, SW_SHOW);
            ShowWindow(mw->_bt_prev_chapter->_baseWindow._hWnd, SW_SHOW);

            ShowWindow(mw->_tx_search->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_bt_search->_baseWindow._hWnd, SW_HIDE);
            ShowWindow(mw->_lv_result->_baseWindow._hWnd, SW_HIDE);

            ShowWindow(mw->_richEdit_sy->_baseWindow._hWnd, SW_SHOW);
        }
        break;
    }
    }
}

static void OnNotify_treeView(MainWindow* mw, WPARAM wParam, LPARAM lParam)
{
    switch (((LPNMHDR)lParam)->code)
    {
    case NM_DBLCLK:
        OnDBClick_treeView(mw, wParam, lParam);
        break;
    }
}

static void OnPaint(MainWindow* mw)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(mw->_baseWindow._hWnd, &ps);

    //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOWFRAME));

    EndPaint(mw->_baseWindow._hWnd, &ps);
}

static void OnDestroy(MainWindow* mw)
{
    PostQuitMessage(0);
}

static LRESULT HandleMessage(BaseWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* mw = (MainWindow*)_this;

    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(mw);
        return 0;

    case WM_PAINT:
        OnPaint(mw);
        return 0;

    case WM_SIZING:
        return mw->_baseWindow._OnSizingFunc(&mw->_baseWindow, (RECT*)lParam);

    case WM_SIZE:
        OnSize(mw, LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->hwndFrom == mw->_treeView->_baseWindow._hWnd)
            OnNotify_treeView(mw, wParam, lParam);
        else if (((LPNMHDR)lParam)->hwndFrom == mw->_tabControl->_baseWindow._hWnd)
            OnNotify_tabControl(mw, wParam, lParam);
        return 0;

    case WM_DESTROY:
        OnDestroy(mw);
        return 0;

    default:
        return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);
    }
}

LRESULT TabControlCallBckProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN64
    MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
    MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
#endif

    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_BUTTON_NEXT_CHAPTER:
            OnBtnClicked_next_chapter(mw, wParam, lParam);
            break;

        case ID_BUTTON_PREV_CHAPTER:
            OnBtnClicked_prev_chapter(mw, wParam, lParam);
            break;

        case ID_BUTTON_SEARCH:
            OnBtnClicked_search(mw, wParam, lParam);
            break;
        
        default:
            break;
        }
        break;
    default:
        break;
    }
    return CallWindowProc(lpTabControlWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT TextEditCallBckProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN64
    MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
    MainWindow* mw = (MainWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
#endif

    switch (uMsg)
    {
    case WM_CHAR:
    {
        if (wParam >= L'0' && wParam <= L'9')
        {
            wParam -= L'0';
            wParam += 0x0660;
        }
    }
        break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_RETURN:
            OnTXChaper_enter(mw, wParam, lParam);
            return 0;
        }
    }
        break;
    default:
        break;
    }
    return CallWindowProc(lpTextEditWndProc, hWnd, uMsg, wParam, lParam);
}
