# gdb pretty-printer contributed by @bas524, Alexander B.
# 
# register with:
#   libstdcxx_printer.add_version('nonstd::optional_lite::', 'optional', NonStdOptionalPrinter)

class NonStdOptionalPrinter(SingleObjContainerPrinter):
    "Print a nonstd::optional"

    def __init__ (self, typename, val):
        alternatives = get_template_arg_list(val.type)
        valtype = self._recognize (val.type.template_argument(0))
        self.typename = strip_versioned_namespace(typename)
        self.typename = re.sub('^nonstd::(optional_lite::|)(optional::|)(.*)', r'nonstd::\1\3<%s>' % valtype, self.typename, 1)
        self.val = val
        self.contained_type = alternatives[0]
        addr = val['contained']['data']['__data'].address
        contained_value = addr.cast(self.contained_type.pointer()).dereference()
        visualizer = gdb.default_visualizer (contained_value)
        super (NonStdOptionalPrinter, self).__init__ (contained_value, visualizer)

    def to_string (self):
        if self.contained_value is None:
            return "%s [no contained value]" % self.typename
        if self.visualizer:
            return "%s containing %s" % (self.typename, self.visualizer.to_string())
        return self.typename
