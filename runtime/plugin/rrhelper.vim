" Vim plugin with helper function(s) for --remote-wait
" Maintainer: Flemming Madsen <fma@cci.dk>
" Last Change: 2001 Jul 28

if has("xcmdsrv")
  function SetupRemoteReplies()
    let cnt = 0
    let max = argc()

    let ID = expand("<client>")
    augroup RemoteReplyGroup
    while cnt < max
      execute 'autocmd RemoteReplyGroup BufUnload '.argv(cnt)
	\.' call serverreply_send("'.ID.'", "'.cnt.'")'
	\.'| autocmd! RemoteReplyGroup BufUnload '.argv(cnt)
      let cnt = cnt + 1
    endwhile
    augroup END
  endfunc
endif

" vim: set sw=2 sts=2 :
