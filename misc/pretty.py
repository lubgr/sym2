
import lldb

def exprViewSummary(valobj, unused):
    return "" #TODO

def exprSummary(valobj, unused):
    return "" #TODO

class ExprViewSynthProvider:
    def __init__(self, value, internalDict):
        self.value = value

    def num_children(self):
        return self.size

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        try:
            offset = index * self.dataSize
            return self.data.CreateChildAtOffset('[' + str(index) + ']', offset, self.valueType)
        except:
            return None

    def update(self):
        first = self.value.GetChildMemberWithName('first').GetChildMemberWithName('b')
        last = self.value.GetChildMemberWithName('sentinel').GetChildMemberWithName('b')

        self.data = first
        self.valueType = self.data.GetType().GetPointeeType()
        self.dataSize = self.valueType.GetByteSize()
        self.size = int((last.GetValueAsUnsigned() - first.GetValueAsUnsigned())/self.dataSize)

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

def isLargeIntBlob(valobj, headerName, nOps, nChildBlobs):
    # Nothing more than a heuristic that seems to work for expressions with not too many operands. It's not bullet
    # proof, and if it fails, and a Blob might be pretty-printed as a meaningless operand or (worse), an operand is
    # displayed as large integer limb data.
    pre = valobj.GetChildMemberWithName('pre')
    mid = valobj.GetChildMemberWithName('mid')
    data = valobj.GetChildMemberWithName('main')
    name1 = pre.GetChildMemberWithName('name')
    name2 = mid.GetChildMemberWithName('name')
    sign = mid.GetChildMemberWithName('largeIntSign').GetValueAsUnsigned(0)
    shouldHaveName = headerName in ['symbol', 'constant', 'functionId']

    if allZeroBytes(name1.GetData(), 2) and allZeroBytes(name2.GetData(), 4) and not shouldHaveName:
        return False
    elif headerName in ['largeRational', 'power', 'complexNumber'] and nOps == 2:
        return False
    elif headerName == 'function' and nOps in [1, 2]:
        return False
    elif headerName in ['sum', 'product'] and nOps < 150 and nChildBlobs < 250:
        return False
    elif headerName == 'largeInt' and sign == 1 or sign == -1:
        return False
    elif shouldHaveName:
        return False

    return True

def nameFrom(unionsWithName):
    # It's apparently sufficient to just read out the first char parts as the string is
    # interpreted as zero-terminated:
    return unionsWithName[0].GetChildMemberWithName('name').GetSummary()

def blobSummary(valobj, unused):
    headerName = enumMemberString(valobj, 'header')
    flagsName = enumMemberString(valobj, 'flags')
    pre = valobj.GetChildMemberWithName('pre')
    mid = valobj.GetChildMemberWithName('mid')
    data = valobj.GetChildMemberWithName('main')
    nOps = mid.GetChildMemberWithName('nLogicalOrPhysicalChildren').GetValueAsUnsigned(0)
    nChildBlobs = data.GetChildMemberWithName('nChildBlobs').GetValueAsUnsigned(0)

    if isLargeIntBlob(valobj, headerName, nOps, nChildBlobs):
        return "(Large int blob)"

    err = lldb.SBError()
    isComposite = False
    rep = ""

    if headerName in ['smallInt', 'smallRational']:
        exact = data.GetChildMemberWithName('exact')
        num = exact.GetChildMemberWithName('num').GetValueAsSigned(0)
        denom = exact.GetChildMemberWithName('denom').GetValueAsSigned(0)
        rep = '%d' % num + ('' if denom == 1 else '/%d' % denom)
    elif headerName == 'floatingPoint':
        inexact = data.GetChildMemberWithName('inexact')
        value = inexact.GetData().GetDouble(err, 0)
        rep = '%f' % value
    elif headerName in ['symbol']:
        rep = '%s' % nameFrom([pre, mid, data])
    elif headerName == 'constant':
        name = nameFrom([pre, mid])
        inexact = data.GetChildMemberWithName('inexact')
        value = inexact.GetData().GetDouble(err, 0)
        rep = '%s: %f' % (name, value)
    elif headerName == 'functionId':
        name = nameFrom([pre, mid])
        fctKey = 'unaryEval' if nOps == 1 else 'binaryEval'
        fct = data.GetChildMemberWithName(fctKey).GetSummary()
        rep = '%s %s' % (name, fct)
    elif headerName == 'largeInt':
        rep = 'Large int, blobs: %s' % nChildBlobs
    else:
        isComposite = True
        rep = '%d/%d' % (nOps, nChildBlobs)

    if isComposite:
        return '%s, size: %s (flags: %s)' % (headerName, rep, flagsName)
    else:
        return rep

def __lldb_init_module(debugger, internalDict):
    debugger.HandleCommand('type summary add -x "^sym2::ExprView$" -e -F pretty.exprViewSummary')
    debugger.HandleCommand('type summary add -x "^sym2::Expr$" -e -F pretty.exprSummary')
    debugger.HandleCommand('type summary add -x "^sym2::Blob$" -F pretty.blobSummary')
    debugger.HandleCommand('type synthetic add -x "^sym2::ExprView$" -l pretty.ExprViewSynthProvider')
