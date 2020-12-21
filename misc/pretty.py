
import lldb

def exprViewSummary(valobj, unused):
    return "" #TODO

def exprSummary(valobj, unused):
    return "" #TODO

def enumMemberString(valobj, memberName):
    value = valobj.GetChildMemberWithName(memberName)
    # It would be cleaner to retrieve the value as an integer and then index into GetType().GetEnumMembers(),
    # but that doesn't work with bitfields, hence the stringification:
    return str(value).split()[-1]

def allZeroBytes(data, n):
    err = lldb.SBError()

    for i in range(0, n):
        if data.GetUnsignedInt8(err, i) != 0:
            return False

    return True

def isLargeIntBlob(valobj):
    headerName = enumMemberString(valobj, 'header')
    name = valobj.GetChildMemberWithName('pre').GetChildMemberWithName('name')
    sign = valobj.GetChildMemberWithName('pre').GetChildMemberWithName('largeIntSign')
    shouldHaveName = headerName in ['symbol', 'constant', 'function']
    err = lldb.SBError()
    signValue = sign.GetData().GetUnsignedInt8(err, 0)

    if allZeroBytes(name.GetData(), 6) and not shouldHaveName:
        return False
    elif headerName == 'largeInt' and signValue == 1 or signValue == -1:
        return False
    elif shouldHaveName:
        return False

    return True

def operandSummary(valobj, unused):
    if isLargeIntBlob(valobj):
        return "(Large int blob)"

    headerName = enumMemberString(valobj, 'header')
    flagsName = enumMemberString(valobj, 'flags')
    data = valobj.GetChildMemberWithName('main')
    pre = valobj.GetChildMemberWithName('pre')
    err = lldb.SBError()
    rep = ""

    if headerName in ['smallInt', 'smallRational']:
        exact = data.GetChildMemberWithName('exact')
        num = exact.GetChildMemberWithName('num').GetValueAsUnsigned(0)
        denom = exact.GetChildMemberWithName('denom').GetValueAsUnsigned(0)
        rep = '%d' % num + ('' if denom == 1 else '/%d' % denom)
    elif headerName == 'floatingPoint':
        inexact = data.GetChildMemberWithName('inexact')
        value = inexact.GetData().GetDouble(err, 0)
        rep = '%f' % value
    elif headerName in ['symbol']:
        name = [pre.GetChildMemberWithName('name'), data.GetChildMemberWithName('name')]
        rep = '"%s%s"' % (name[0].GetData().GetString(err, 0), name[1].GetData().GetString(err, 0))
    elif headerName == 'constant':
        name = pre.GetChildMemberWithName('name').GetData().GetString(err, 0)
        inexact = data.GetChildMemberWithName('inexact')
        value = inexact.GetData().GetDouble(err, 0)
        rep = '"%s: %f (constant)"' % (name, value)
    else:
        count = data.GetChildMemberWithName('count').GetValueAsUnsigned(0)
        rep = '%s: %d' % (headerName, count)

    return '%s (%s, %s)' % (rep, headerName, flagsName)

def __lldb_init_module(debugger, internalDict):
    debugger.HandleCommand('type summary add -x "^sym2::ExprView$" -e -F pretty.exprViewSummary')
    debugger.HandleCommand('type summary add -x "^sym2::Expr$" -e -F pretty.exprSummary')
    debugger.HandleCommand('type summary add -x "^sym2::Operand$" -F pretty.operandSummary')
