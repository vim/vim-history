" Vim syntax file
" Language:	HTML
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/html.vim
" Last change:	1998 Jul 22

" Please check :help html.vim for some comments and a description of the options

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" mark illegal characters
syn match htmlError "[<>&]"

if !exists("main_syntax")
  let main_syntax = 'html'
endif

" tags
syn match   htmlSpecial  contained "\\\d\d\d\|\\."
syn region  htmlString   contained start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=htmlSpecial,javaScriptExpression
syn region  htmlString   contained start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=htmlSpecial,javaScriptExpression
syn match   htmlValue    contained "=[\t ]*[^'" \t>][^ \t>]*"hs=s+1   contains=javaScriptExpression
syn region  htmlEndTag             start=+</+    end=+>+              contains=htmlTagN,htmlTagError
syn region  htmlTag                start=+<[^/]+ end=+>+              contains=htmlTagN,htmlString,htmlArg,htmlValue,htmlTagError,htmlEvent,htmlCssDefinition
syn match   htmlTagN     contained +<\s*[-a-zA-Z0-9]\++ms=s+1 contains=htmlTagName,htmlSpecialTagName
syn match   htmlTagN     contained +</\s*[-a-zA-Z0-9]\++ms=s+2 contains=htmlTagName,htmlSpecialTagName
syn match   htmlTagError contained "[^>]<"ms=s+1

" tag names
syn keyword htmlTagName contained address applet area a base basefont
syn keyword htmlTagName contained big blockquote br caption center
syn keyword htmlTagName contained cite code dd dfn dir div dl dt font
syn keyword htmlTagName contained form hr html img
syn keyword htmlTagName contained input isindex kbd li link map menu
syn keyword htmlTagName contained meta ol option param pre p samp span
syn keyword htmlTagName contained select small strike sub sup
syn keyword htmlTagName contained table td textarea th tr tt ul var xmp
syn match htmlTagName contained "\<\(b\|i\|u\|h[1-6]\|em\|strong\|head\|body\|title\)\>"

" legal arg names
syn keyword htmlArg contained action
syn keyword htmlArg contained align alink alt archive background bgcolor
syn keyword htmlArg contained border bordercolor cellpadding
syn keyword htmlArg contained cellspacing checked class clear code codebase color
syn keyword htmlArg contained cols colspan content coords enctype face
syn keyword htmlArg contained gutter height hspace id
syn keyword htmlArg contained link lowsrc marginheight
syn keyword htmlArg contained marginwidth maxlength method name prompt
syn keyword htmlArg contained rel rev rows rowspan scrolling selected shape
syn keyword htmlArg contained size src start target text type url
syn keyword htmlArg contained usemap ismap valign value vlink vspace width wrap
syn match   htmlArg contained "http-equiv"
syn match   htmlArg contained "\<\(href\|title\)="me=e-1

" Netscape extensions
syn keyword htmlTagName contained frame frameset nobr
syn keyword htmlTagName contained layer ilayer nolayer spacer
syn keyword htmlArg     contained frameborder noresize pagex pagey above below
syn keyword htmlArg     contained left top visibility clip id noshade
syn match   htmlArg     contained "z-index"

" special characters
syn match htmlSpecialChar "&[^;]*;"

" Comments (the real ones or the old netscape ones)
if exists("html_wrong_comments")
  syn region htmlComment                start=+<!--+      end=+-->+
else
  syn region htmlComment                start=+<!+        end=+>+   contains=htmlCommentPart,htmlCommentError
  syn match  htmlCommentError contained "[^><!]"
  syn region htmlCommentPart  contained start=+--+        end=+--+
endif
syn region htmlComment                start=+<!DOCTYPE+ keepend end=+>+

" server-parsed commands
syn region htmlPreProc start=+<!--#+ end=+-->+

if !exists("html_no_rendering")
  " rendering
  syn cluster htmlTop contains=htmlTag,htmlEndTag,htmlSpecialChar,htmlPreProc,htmlComment,htmlLink,javaScript

  syn region htmlBold start="<b\>" end="</b>"me=e-4 contains=@htmlTop,htmlBoldUnderline,htmlBoldItalic
  syn region htmlBold start="<strong\>" end="</strong>"me=e-9 contains=@htmlTop,htmlBoldUnderline,htmlBoldItalic
  syn region htmlBoldUnderline contained start="<u\>" end="</u>"me=e-4 contains=@htmlTop,htmlBoldUnderlineItalic
  syn region htmlBoldItalic contained start="<i\>" end="</i>"me=e-4 contains=@htmlTop,htmlBoldItalicUnderline
  syn region htmlBoldItalic contained start="<em\>" end="</em>"me=e-5 contains=@htmlTop,htmlBoldItalicUnderline
  syn region htmlBoldUnderlineItalic contained start="<i\>" end="</i>"me=e-4 contains=@htmlTop
  syn region htmlBoldUnderlineItalic contained start="<em\>" end="</em>"me=e-5 contains=@htmlTop
  syn region htmlBoldItalicUnderline contained start="<u\>" end="</u>"me=e-4 contains=@htmlTop,htmlBoldUnderlineItalic

  syn region htmlUnderline start="<u\>" end="</u>"me=e-4 contains=@htmlTop,htmlUnderlineBold,htmlUnderlineItalic
  syn region htmlUnderlineBold contained start="<b\>" end="</b>"me=e-4 contains=@htmlTop,htmlUnderlineBoldItalic
  syn region htmlUnderlineBold contained start="<strong\>" end="</strong>"me=e-9 contains=@htmlTop,htmlUnderlineBoldItalic
  syn region htmlUnderlineItalic contained start="<i\>" end="</i>"me=e-4 contains=@htmlTop,htmUnderlineItalicBold
  syn region htmlUnderlineItalic contained start="<em\>" end="</em>"me=e-5 contains=@htmlTop,htmUnderlineItalicBold
  syn region htmlUnderlineItalicBold contained start="<b\>" end="</b>"me=e-4 contains=@htmlTop
  syn region htmlUnderlineItalicBold contained start="<strong\>" end="</strong>"me=e-9 contains=@htmlTop
  syn region htmlUnderlineBoldItalic contained start="<i\>" end="</i>"me=e-4 contains=@htmlTop
  syn region htmlUnderlineBoldItalic contained start="<em\>" end="</em>"me=e-5 contains=@htmlTop

  syn region htmlItalic start="<i\>" end="</i>"me=e-4 contains=@htmlTop,htmlItalicBold,htmlItalicUnderline
  syn region htmlItalic start="<em\>" end="</em>"me=e-5 contains=@htmlTop
  syn region htmlItalicBold contained start="<b\>" end="</b>"me=e-4 contains=@htmlTop,htmlItalicBoldUnderline
  syn region htmlItalicBold contained start="<strong\>" end="</strong>"me=e-9 contains=@htmlTop,htmlItalicBoldUnderline
  syn region htmlItalicBoldUnderline contained start="<u\>" end="</u>"me=e-4 contains=@htmlTop
  syn region htmlItalicUnderline contained start="<u\>" end="</u>"me=e-4 contains=@htmlTop,htmlItalicUnderlineBold
  syn region htmlItalicUnderlineBold contained start="<b\>" end="</b>"me=e-4 contains=@htmlTop
  syn region htmlItalicUnderlineBold contained start="<strong\>" end="</strong>"me=e-9 contains=@htmlTop

  syn region htmlLink start="<a\>[^>]*href\>" end="</a>"me=e-4 contains=htmlTag,htmlEndTag,htmlSpecialChar,htmlPreProc,htmlComment,javaScript
  syn region htmlH1 start="<h1\>" end="</h1>"me=e-5 contains=@htmlTop
  syn region htmlH2 start="<h2\>" end="</h2>"me=e-5 contains=@htmlTop
  syn region htmlH3 start="<h3\>" end="</h3>"me=e-5 contains=@htmlTop
  syn region htmlH4 start="<h4\>" end="</h4>"me=e-5 contains=@htmlTop
  syn region htmlH5 start="<h5\>" end="</h5>"me=e-5 contains=@htmlTop
  syn region htmlH6 start="<h6\>" end="</h6>"me=e-5 contains=@htmlTop
  syn region htmlHead start="<head\>" end="</head>"me=e-7 end="<body\>"me=e-5 contains=htmlTag,htmlEndTag,htmlSpecialChar,htmlPreProc,htmlComment,htmlLink,htmlTitle,javaScript,cssStyle
  syn region htmlTitle start="<title\>" end="</title>"me=e-8 contains=htmlTag,htmlEndTag,htmlSpecialChar,htmlPreProc,htmlComment,javaScript
endif

if main_syntax != 'java' || exists("java_javascript")
  " JAVA SCRIPT
  syn keyword htmlTagName                contained noscript
  syn keyword htmlSpecialTagName         contained script style

  syn include @htmlJavaScript <sfile>:p:h/javascript.vim
  syn region  javaScript start=+<script[^>]*>.+ keepend end=+</script>+me=s-1 contains=@htmlJavaScript,htmlCssStyleComment,htmlScriptTag
  syn region  htmlScriptTag contained start=+<script+ end=+>+              contains=htmlTagN,htmlString,htmlArg,htmlValue,htmlTagError,htmlEvent
  hi link htmlScriptTag htmlTag

  " html events (i.e. arguments that include javascript commands)
  syn region htmlEvent        contained start=+on\a\+\s*=[\t ]*'+ end=+'+ keepend contains=htmlEventSQ
  syn region htmlEventSQ	    contained start=+'+ms=s+1 end=+'+me=s-1 contains=@htmlJavaScript
  syn region htmlEvent        contained start=+on\a\+\s*=[\t ]*"+ end=+"+ keepend contains=htmlEventDQ
  syn region htmlEventDQ	    contained start=+'+ms=s+1 end=+'+me=s-1 contains=@htmlJavaScript
  hi link htmlEventSQ htmlEvent
  hi link htmlEventDQ htmlEvent

  " a javascript expression is used as an arg value
  syn region  javaScriptExpression contained                start=+&{+ keepend end=+};+ contains=@htmlJavaScript
endif

if main_syntax != 'java' || exists("java_css")
  " embedded style sheets
  syn keyword htmlArg                    contained media
  syn include @htmlCss <sfile>:p:h/css.vim
  syn region cssStyle start=+<style+ keepend end=+</style>+ contains=@htmlCss,htmlTag,htmlEndTag,htmlCssStyleComment
  syn match htmlCssStyleComment contained "\(<!--\|-->\)"
  syn region htmlCssDefinition matchgroup=htmlArg start='style="' keepend matchgroup=htmlString end='"' contains=css.*Attr,css.*Properties,cssComment,cssLength,cssColor,cssURL,cssImportant,cssError,cssString
  hi link htmlStyleArg htmlString
endif

if main_syntax == "html"
  " synchronizing (does not always work if a comment includes legal
  " html tags, but doing it right would mean to always start
  " at the first line, which is too slow)
  syn sync match htmlHighlight groupthere NONE "<[/a-zA-Z]"
  syn sync match htmlHighlight groupthere javaScript "<script"
  syn sync match htmlHighlightSkip "^.*['\"].*$"
  syn sync minlines=10
endif

if !exists("did_html_syntax_inits")
  let did_html_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link htmlTag			Function
  hi link htmlEndTag			Identifier
  hi link htmlArg			Type
  hi link htmlTagName			htmlStatement
  hi link htmlSpecialTagName		Exception
  hi link htmlValue			Value
  hi link htmlSpecialChar		Special

  if !exists("html_no_rendering")
    hi link htmlH1			Title
    hi link htmlH2			htmlH1
    hi link htmlH3			htmlH2
    hi link htmlH4			htmlH3
    hi link htmlH5			htmlH4
    hi link htmlH6			htmlH5
    hi link htmlHead			PreProc
    hi link htmlTitle			Title
    hi link htmlBoldItalicUnderline	htmlBoldUnderlineItalic
    hi link htmlUnderlineBold		htmlBoldUnderline
    hi link htmlUnderlineItalicBold	htmlBoldUnderlineItalic
    hi link htmlUnderlineBoldItalic	htmlBoldUnderlineItalic
    hi link htmlItalicUnderline		htmlUnderlineItalic
    hi link htmlItalicBold		htmlBoldItalic
    hi link htmlItalicBoldUnderline	htmlBoldUnderlineItalic
    hi link htmlItalicUnderlineBold	htmlBoldUnderlineItalic
    if !exists("html_my_rendering")
      if &background == "dark"
	hi htmlLink		 term=underline cterm=underline ctermfg=cyan gui=underline guifg=#80a0ff
      else
	hi htmlLink		 term=underline cterm=underline ctermfg=DarkBlue gui=underline guifg=Blue
      endif
      hi htmlBold		 term=bold cterm=bold gui=bold
      hi htmlBoldUnderline	 term=bold,underline cterm=bold,underline gui=bold,underline
      hi htmlBoldItalic		 term=bold,italic cterm=bold,italic gui=bold,italic
      hi htmlBoldUnderlineItalic term=bold,italic,underline cterm=bold,italic,underline gui=bold,italic,underline
      hi htmlUnderline		 term=underline cterm=underline gui=underline
      hi htmlUnderlineItalic	 term=italic,underline cterm=italic,underline gui=italic,underline
      hi htmlItalic		 term=italic cterm=italic gui=italic
    endif
  endif

  hi link htmlSpecial			Special
  hi link htmlSpecialChar		Special
  hi link htmlString			String
  hi link htmlStatement			Statement
  hi link htmlComment			Comment
  hi link htmlCommentPart		Comment
  hi link htmlPreProc			PreProc
  hi link htmlValue			String
  hi link htmlCommentError		htmlError
  hi link htmlTagError			htmlError
  hi link htmlEvent			javaScript
  hi link htmlError			Error

  hi link javaScript			Special
  hi link javaScriptExpression		javaScript
  hi link htmlCssStyleComment		Comment
  hi link htmlCssDefinition		Special
endif

let b:current_syntax = "html"

if main_syntax == 'html'
  unlet main_syntax
endif

" vim: ts=8
