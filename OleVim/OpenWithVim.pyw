# OpenWithVim, Python version (beta)
# nearly the same as OpenWithVim, VB version
#
# note: in order to run, you must have installed Python
#       on your system!
#
# differences to VB version: you can specify more than one
# filename as parameters
#
# author: Christian Schaller (Christian.Schaller@mchp.siemens.de)

import getopt
import sys
import string
import win32com.client

_name = sys.argv[0]

def usage():
    print 'OpenWithVim (Python version)'
    print 'usage:', _name, '[switches] [files]'
    print 'switches:'
    print '\t+<linenr>  goto line number <linenr>'
    print '\t-r<key>    use <key> for restore window'
    print '\t-? -h      show this help'

try:
    optlist, args = getopt.getopt(sys.argv[1:], 'h?r:')
except getopt.error, e:
    print e
    usage()
    sys.exit()

if ('-h', '') in optlist or ('-?', '') in optlist:
    usage()
    sys.exit()

# get dispatch for vim
vim = win32com.client.Dispatch('Vim.Application')
vim.SendKeys('<c-\><c-n>')

vimCmd = ':'
vimArgs = []
lineNr = ''
# filter linenumber arg from args
for arg in args:
    if arg[0] == '+':
        lineNr = arg[1:]
    else:
        vimArgs.append(arg)

# edit file(s)
if len(vimArgs) > 1:
    vimCmd = vimCmd + 'n ' + string.join(vimArgs) + ' <bar> '
elif len(vimArgs) > 0:
    vimCmd = vimCmd + 'e ' + vimArgs[0] + ' <bar> '

for switch, value in optlist:
    if switch == '-r':
        vimCmd = vimCmd + 'simalt ~' + value + ' <bar> '

# don't send the following two commands with one SendKey()!
# there's something wrong with it...
vim.SendKeys(vimCmd)
vim.SendKeys('<c-m>')

if lineNr != '':
    vim.SendKeys(':' + lineNr + '<c-m>')

vim.SetForeground()
