" Menu Translations:	Traditional Chinese (for Big5 UNIX/Windows)
" Translated By: 	Hung-teh, Lin	<piaip@csie.ntu.edu.tw>
" Last Change:		2000/11/11 16:00:00 [for VIM 6.0l]

menutrans clear
let did_menu_trans = 1

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
menutrans Save\ &As\.\.\.<Tab>:w	另存新檔(&A)\.\.\.<Tab>:w
menutrans &Print			列印(&P)
menutrans Sa&ve-Exit<Tab>:wqa		儲存並離開(&V)<Tab>:wqa
menutrans E&xit<Tab>:qa			離開(&X)<Tab>:qa

" Edit menu
menutrans &Edit				編輯(&E)
menutrans &Undo<Tab>u			復原(&U)<Tab>u
menutrans &Redo<Tab>^R			取消上次復原(&R)<Tab>^R
menutrans Rep&eat<Tab>\.		重複上次動作(&E)<Tab>\.
menutrans Cu&t<Tab>"*x			剪下(&T)<Tab>"*x
menutrans &Copy<Tab>"*y			複製(&C)<Tab>"*y
menutrans &Paste<Tab>"*p		貼上(&P)<Tab>"*p
menutrans Put\ &Before<Tab>[p		貼到游標前(&B)<Tab>[p
menutrans Put\ &After<Tab>]p		貼到游標後(&A)<Tab>]p
menutrans &Delete<Tab>x			刪除(&D)<Tab>x
menutrans &Select\ all<Tab>ggVG		全選(&S)<Tab>ggvG
menutrans &Find\.\.\.			尋找(&F)\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.	尋找並取代(&L)\.\.\.
menutrans Options\.\.\.			我的喜好設定(&O)\.\.\.

" Programming menu
menutrans &Tools			工具(&T)
menutrans &Jump\ to\ this\ tag<Tab>g^]	檢索游標處的標籤關鍵字[tag](&J)<Tab>g^]
menutrans Jump\ &back<Tab>^T		跳回檢索前的位置(&B)<Tab>^T
menutrans Build\ &Tags\ File		建立標籤索引檔\ Tags(&T)
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

" Names for buffer menu.
menutrans &Buffers	緩衝區(&B)
menutrans Refresh	更新
menutrans Delete	刪除
menutrans Alternate	替換
menutrans [No\ File]	[無檔案]

" Window menu
menutrans &Window			視窗(&W)
menutrans &New<Tab>^Wn			開新視窗(&N)<Tab>^Wn
menutrans S&plit<Tab>^Ws		分割視窗(&P)<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^	分割到#(&L)<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	垂直分割(&V)<Tab>^Wv
menutrans &Close<Tab>^Wc		關閉視窗(&C)<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo	關閉其它視窗(&O)<Tab>^Wo
menutrans Ne&xt<Tab>^Ww			下一個(&X)<Tab>^Ww
menutrans P&revious<Tab>^WW		上一個(&R)<Tab>^WW
menutrans &Equal\ Height<Tab>^W=	所有視窗等高(&E)<Tab>^W=
menutrans &Max\ Height<Tab>^W_		最大高度(&M)<Tab>^W
menutrans M&in\ Height<Tab>^W1_		最小高度(&i)<Tab>^W1_
menutrans Max\ Width<Tab>^W\|		最大寬度<Tab>^W\|
menutrans Min\ Width<Tab>^W1\|		最小寬度<Tab>^W1\|
menutrans Rotate\ &Up<Tab>^WR		上移視窗(&U)<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr		下移視窗(&D)<Tab>^Wr
menutrans Select\ &Font\.\.\.		設定字型(&F)\.\.\.
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
" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
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
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New		開新視窗
      tmenu ToolBar.WinSplit	分割視窗
      tmenu ToolBar.WinMax	最大化
      tmenu ToolBar.WinMin	最小化
      tmenu ToolBar.WinClose	關閉視窗
    endif
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
menutrans Set\ 'filetype'\ too	也要設定\ 'filetype'
menutrans &Off			關閉(&O)
menutrans &Manual		手動設定(&M)
menutrans A&utomatic		自動設定(&U)
menutrans o&n\ (this\ file)	開啟(&N)\ (只對這個檔)
menutrans o&ff\ (this\ file)	關閉(&F)\ (只對這個檔)
menutrans Co&lor\ test		色彩顯試測試(&L)
menutrans &Highlight\ test	語法效果測試(&H)
menutrans &Convert\ to\ HTML	轉換成\ HTML\ 格式(&C)
