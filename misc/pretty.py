
import lldb

class ExprViewSynthProvider:
    def __init__(self, value, internalDict):
        self.value = value

    def num_children(self):
        return 2

    def get_child_index(self, name):
        return 0

    def get_child_at_index(self, index):
        try:
            if index == 0:
                return self.struc.CreateChildAtOffset('tags: ', 0, self.struc.GetType())
            elif index == 1:
                return self.leaves.CreateChildAtOffset('leaves: ', 0, self.leaves.GetType())
        except:
            return None

    def update(self):
        self.struc = self.value.GetChildMemberWithName('structure')
        self.leaves = self.value.GetChildMemberWithName('leaves')

def opDescSummary(valobj, unused):
    names = { 1 : 'scalar', 1 << 1 : 'sum', 1 << 2 : 'product', 1 << 3 : 'power', 1 << 4 : 'function' }
    info = valobj.GetChildMemberWithName('info').GetValueAsUnsigned()
    count = valobj.GetChildMemberWithName('count').GetValueAsUnsigned()
    return names[info] + "(" + str(count) + ")"

def __lldb_init_module(debugger, internalDict):
    debugger.HandleCommand('type synthetic add -x "^sym2::ExprView$" -l pretty.ExprViewSynthProvider')
    debugger.HandleCommand('type summary add -x "^sym2::OpDesc$" -F pretty.opDescSummary')
    debugger.HandleCommand('type summary add -x "^sym2::Number$" -s "${var.rep%S}"')
