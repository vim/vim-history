" Vim syntax file
" This is a GENERATED FILE. Please always refer to source file at the URI below.
" Language: gkrellm theme files `gkrellmrc'
" Maintainer: David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change: 2002-05-07
" URL: http://physics.muni.cz/~yeti/download/syntax/gkrellmrc.vim

" Setup
if version >= 600
  if exists("b:current_syntax")
    finish
  endif
else
  syntax clear
endif

if version >= 600
  setlocal iskeyword=_,-,a-z,A-Z,48-57
else
  set iskeyword=_,-,a-z,A-Z,48-57
endif

syn case match

" Base constructs
syn match gkrellmrcComment "#.*$" contains=gkrellmrcFixme
syn keyword gkrellmrcFixme FIXME TODO XXX NOT contained
syn region gkrellmrcString start=+"+ skip=+\\\\\|\\"+ end=+"+ oneline
syn match gkrellmrcNumber "^\(\d\+\)\=\.\=\d\+"
syn match gkrellmrcNumber "\W\(\d\+\)\=\.\=\d\+"lc=1
syn keyword gkrellmrcConstant none
syn match gkrellmrcRGBColor "#\(\x\{12}\|\x\{9}\|\x\{6}\|\x\{3}\)\>"

" Keywords
syn keyword gkrellmrcBuiltinExt cpu_nice_color cpu_nice_grid_color krell_depth krell_expand krell_left_margin krell_right_margin krell_x_hot krell_yoff mem_krell_buffers_depth mem_krell_buffers_expand mem_krell_buffers_x_hot mem_krell_buffers_yoff mem_krell_cache_depth mem_krell_cache_expand mem_krell_cache_x_hot mem_krell_cache_yoff sensors_bg_volt timer_bg_timer
syn keyword gkrellmrcGlobal allow_scaling author bg_grid_mode bg_separator_height bg_slider_meter_border bg_slider_panel_border button_meter_border button_panel_border chart_in_color chart_in_color_grid chart_out_color chart_out_color_grid chart_width_ref decal_alarm_frames decal_mail_delay decal_mail_frames decal_warn_frames frame_bottom_border frame_bottom_height frame_left_border frame_left_width frame_right_border frame_right_width frame_top_border frame_top_height krell_slider_depth krell_slider_expand krell_slider_x_hot large_font normal_font rx_led_x rx_led_y small_font spacer_bottom_height spacer_top_height theme_alternatives tx_led_x tx_led_y
syn keyword gkrellmrcSetCmd set_image_border set_integer set_string
syn keyword gkrellmrcExpandMode left right bar-mode left-scaled right-scaled bar-mode-scaled
syn keyword gkrellmrcMeterName apm cal clock fs host mail mem swap timer sensors uptime
syn keyword gkrellmrcChartName cpu proc disk inet and net
syn match gkrellmrcSpecialClassName "\*"
syn keyword gkrellmrcStyleCmd StyleMeter StyleChart StylePanel
syn keyword gkrellmrcStyleItem textcolor alt_textcolor font alt_font transparency border label_position margin margins left_margin right_margin top_margin bottom_margin krell_depth krell_yoff krell_x_hot krell_expand krell_left_margin krell_right_margin

" Define the default highlighting
if version >= 508 || !exists("did_gtkrc_syntax_inits")
  if version < 508
    let did_gtkrc_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink gkrellmrcComment Comment
  HiLink gkrellmrcFixme Todo

  HiLink gkrellmrcString gkrellmrcConstant
  HiLink gkrellmrcNumber gkrellmrcConstant
  HiLink gkrellmrcRGBColor gkrellmrcConstant
  HiLink gkrellmrcExpandMode gkrellmrcConstant
  HiLink gkrellmrcConstant Constant

  HiLink gkrellmrcMeterName gkrellmrcClass
  HiLink gkrellmrcChartName gkrellmrcClass
  HiLink gkrellmrcSpecialClassName gkrellmrcClass
  HiLink gkrellmrcClass Type

  HiLink gkrellmrcGlobal gkrellmrcItem
  HiLink gkrellmrcBuiltinExt gkrellmrcItem
  HiLink gkrellmrcStyleItem gkrellmrcItem
  HiLink gkrellmrcItem Function

  HiLink gkrellmrcSetCmd Special
  HiLink gkrellmrcStyleCmd Statement

  delcommand HiLink
endif

let b:current_syntax = "gkrellmrc"
