" Menu Translations:	Traditional Chinese (for Big5 UNIX/Windows)
" Translated By: 	Hung-teh, Lin	<piaip@csie.ntu.edu.tw>
" Last Change:		Tue Apr 24 19:40:17 2001

" Quit when menu translations have already been done.
if exists("did_menu_trans")
  finish
endif
let did_menu_trans = 1

scriptencoding cp950

" Help menu
menutrans &Help			輔助說明(&H)
menutrans &Overview<Tab><F1>	說明手冊總覽(&O)<Tab><F1>
menutrans &GUI			圖型界面(&G)
menutrans &How-to\ links	如何作\.\.\.(&H)
menutrans &Credits		感謝(&C)
menutrans Co&pying		版權宣告(&P)
menutrans &Version		程式版本資訊(&V)
menutrans &About		關於\ Vim(&A)

" File menu
menutrans &File				檔案(&F)
menutrans &Open\.\.\.<Tab>:e		開啟(&O)\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	分割視窗並開啟(&L)<Tab>:sp
menutrans &New<Tab>:enew		編輯新檔案(&N)<Tab>:enew
menutrans &Close<Tab>:q			關閉檔案(&C)<Tab>:q
menutrans &Save<Tab>:w			儲存(&S)<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav	另存新檔(&A)\.\.\.<Tab>:sav
menutrans Show\ &Diff\ with\.\.\.	比較(&Diff)\.\.\.
menutrans Show\ &Patched\ by\.\.\.	執行&Patch\.\.\.
menutrans &Print			列印(&P)
menutrans Sa&ve-Exit<Tab>:wqa		儲存並離開(&V)<Tab>:wqa
menutrans E&xit<Tab>:qa			離開(&X)<Tab>:qa

" Edit menu
menutrans &Edit				編輯(&E)
menutrans &Undo<Tab>u			復原(&U)<Tab>u
menutrans &Redo<Tab>^R			取消上次復原(&R)<Tab>^R
menutrans Rep&eat<Tab>\.		重複上次動作(&E)<Tab>\.
menutrans Cu&t<Tab>"+x			剪下(&T)<Tab>"+x
menutrans &Copy<Tab>"+y			複製(&C)<Tab>"+y
menutrans &Paste<Tab>"+P		貼上(&P)<Tab>"+P
menutrans Put\ &Before<Tab>[p		貼到游標前(&B)<Tab>[p
menutrans Put\ &After<Tab>]p		貼到游標後(&A)<Tab>]p
menutrans &Delete<Tab>x			刪除(&D)<Tab>x
menutrans &Select\ all<Tab>ggVG		全選(&S)<Tab>ggvG
menutrans &Find\.\.\.			尋找(&F)\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.	尋找並取代(&L)\.\.\.
menutrans Settings\ &Window		設定視窗(&W)
menutrans Se&ttings			設定(&T)

" Build boolean options
menutrans Toggle\ Line\ Numbering<Tab>:set\ nu!	切換顯示行號<Tab>:set\ nu!
menutrans Toggle\ Line\ Wrap<Tab>:set\ wrap!	切換自動折行<Tab>:set\ wrap!
menutrans Toggle\ hlsearch<Tab>:set\ hlsearch!	切換搜尋字串高亮度顯示<Tab>:set\ hlsearch!
menutrans Toggle\ expand-tab<Tab>:set\ et!	切換展開TAB<Tab>:set\ et!
menutrans Toggle\ auto-indent<Tab>:set\ ai!	切換自動縮排<Tab>:set\ ai!

" Build GUI options
menutrans Toggle\ Toolbar		切換使用工具列
menutrans Toggle\ Bottom\ Scrollbar	切換使用底端捲動軸
menutrans Toggle\ Left\ Scrollbar	切換使用左端捲動軸
menutrans Toggle\ Right\ Scrolbar	切換使用右端捲動軸

" Build variable options
menutrans Text\ Width\.\.\. 		文字頁面寬度\.\.\.
menutrans Shiftwidth	 		縮排寬度(Shiftwidth)

" Programming menu
menutrans &Tools			工具(&T)
menutrans &Jump\ to\ this\ tag<Tab>g^]	檢索游標處的標籤關鍵字(tag)(&J)<Tab>g^]
menutrans Jump\ &back<Tab>^T		跳回檢索前的位置(&B)<Tab>^T
menutrans Build\ &Tags\ File		建立標籤索引檔\ Tags(&T)
menutrans &Folding			Folding設定(&F)
menutrans &Make<Tab>:make		執行\ Make(&M)<Tab>:make
menutrans &List\ Errors<Tab>:cl		列出編譯錯誤(&E)<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!	列出所有訊息(&I)<Tab>:cl!
menutrans &Next\ Error<Tab>:cn		下一個編譯錯誤處(&N)<Tab>:cn
menutrans &Previous\ Error<Tab>:cp	上一個編譯錯誤處(&P)<Tab>:cp
menutrans &Older\ List<Tab>:cold	檢視舊錯誤列表(&O)<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew	檢視新錯誤列表(&E)<Tab>:cnew
menutrans Error\ &Window<Tab>:cwin	錯誤訊息視窗(&W)<Tab>:cwin	
menutrans Convert\ to\ HEX<Tab>:%!xxd	轉換成16進位碼<Tab>:%!xxd
menutrans Convert\ back<Tab>:%!xxd\ -r	從16進位碼轉換回文字<Tab>:%!xxd\ -r

" Tools.Fold Menu
menutrans &Enable/Disable\ folds<Tab>zi		使用/不使用Folding(&E)<Tab>zi
menutrans &View\ Cursor\ Line<Tab>zv		檢視此行(&V)<Tab>zv
menutrans Vie&w\ Cursor\ Line\ only<Tab>zMzx	只檢視此行(&W)<Tab>zMzx
menutrans C&lose\ more\ folds<Tab>zm		收起Folds(&L)<Tab>zm
menutrans &Close\ all\ folds<Tab>zM		收起所有Folds(&C)<Tab>zM
menutrans O&pen\ more\ folds<Tab>zr		打開Folds(&P)<Tab>zr
menutrans &Open\ all\ folds<Tab>zR		打開所有Folds(&O)<Tab>zR
" fold method
menutrans Fold\ Met&hod				Fold方式(&H)
menutrans Create\ &Fold<Tab>zf			建立Fold(&F)<Tab>zf
menutrans &Delete\ Fold<Tab>zd			刪除Fold(&D)<Tab>zd
menutrans Delete\ &All\ Folds<Tab>zD		刪除所有Fold(&A)<Tab>zD
" moving around in folds
menutrans Fold\ column\ &width			設定Fold欄寬(&W)

" Names for buffer menu.
menutrans &Buffers		緩衝區(&B)
menutrans &Refresh\ menu	更新(&R)
menutrans &Delete		刪除(&D)
menutrans A&lternate		替換(&L)
menutrans &Next			下一個(&N)
menutrans &Previous		前一個(&P)
menutrans [No\ File]		[無檔案]

" Window menu
menutrans &Window			視窗(&W)
menutrans &New<Tab>^Wn			開新視窗(&N)<Tab>^Wn
menutrans S&plit<Tab>^Ws		分割視窗(&P)<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	分割到#(&L)<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	垂直分割(&V)<Tab>^Wv
menutrans Split\ File\ E&xplorer	檔案總管式分割(&X)
menutrans &Close<Tab>^Wc		關閉視窗(&C)<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	關閉其它視窗(&O)<Tab>^Wo
menutrans Move\ &To			移至(&T)
menutrans &Top<Tab>^WK			頂端(&T)<Tab>^WK		
menutrans &Bottom<Tab>^WJ		底端(&B)<Tab>^WJ		
menutrans &Left\ side<Tab>^WH		左邊(&L)<Tab>^WH		
menutrans &Right\ side<Tab>^WL		右邊(&R)<Tab>^WL		
" menutrans Ne&xt<Tab>^Ww		下一個(&X)<Tab>^Ww
" menutrans P&revious<Tab>^WW		上一個(&R)<Tab>^WW
menutrans Rotate\ &Up<Tab>^WR		上移視窗(&U)<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		下移視窗(&D)<Tab>^Wr
menutrans &Equal\ Size<Tab>^W=		所有視窗等高(&E)<Tab>^W=
menutrans &Max\ Height<Tab>^W_		最大高度(&M)<Tab>^W
menutrans M&in\ Height<Tab>^W1_		最小高度(&i)<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		最大寬度<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		最小寬度<Tab>^W1\|
menutrans Select\ Fo&nt\.\.\.		設定字型(&F)\.\.\.
"
" The popup menu
menutrans &Undo			復原(&U)
menutrans Cu&t			剪下(&T)
menutrans &Copy			複製(&C)
menutrans &Paste		貼上(&P)
menutrans &Delete		刪除(&D)
menutrans Select\ Blockwise 	Blockwise式選擇
menutrans Select\ &Word		選擇單字(&W)
menutrans Select\ &Line		選擇行(&L)
menutrans Select\ &Block	選擇區塊(&B)
menutrans Select\ &All		全選(&A)
" 
" The GUI toolbar
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open		開啟檔案
    tmenu ToolBar.Save		儲存目前編輯中的檔案
    tmenu ToolBar.SaveAll	儲存全部檔案
    tmenu ToolBar.Print		列印
    tmenu ToolBar.Undo		復原上次變動
    tmenu ToolBar.Redo		取消上次復原動作
    tmenu ToolBar.Cut		剪下至剪貼簿
    tmenu ToolBar.Copy		複製到剪貼簿
    tmenu ToolBar.Paste		由剪貼簿貼上
    tmenu ToolBar.Find		尋找...
    tmenu ToolBar.FindNext	找下一個
    tmenu ToolBar.FindPrev	找上一個
    tmenu ToolBar.Replace	取代...
    tmenu ToolBar.LoadSesn	載入 Session
    tmenu ToolBar.SaveSesn	儲存目前的 Session
    tmenu ToolBar.RunScript	執行 Vim 程式檔
    tmenu ToolBar.Make		執行 Make
    tmenu ToolBar.Shell		開啟一個命令列視窗 DosBox
    tmenu ToolBar.RunCtags	執行 ctags
    tmenu ToolBar.TagJump	跳到目前游標位置的 tag
    tmenu ToolBar.Help		Vim 輔助說明
    tmenu ToolBar.FindHelp	搜尋 Vim 說明文件
  endfun
endif

" Syntax menu
menutrans &Syntax		語法(&S)
menutrans Set\ 'syntax'\ only	只設定\ 'syntax'
menutrans Set\ 'filetype'\ too	也設定\ 'filetype'
menutrans &Off			關閉(&O)
menutrans &Manual		手動設定(&M)
menutrans A&utomatic		自動設定(&U)
menutrans o&n\ (this\ file)	只對這個檔開啟(&N)
menutrans o&ff\ (this\ file)	只對這個檔關閉(&F)
menutrans Co&lor\ test		色彩顯示測試(&L)
menutrans &Highlight\ test	語法效果測試(&H)
menutrans &Convert\ to\ HTML	轉換成\ HTML\ 格式(&C)
