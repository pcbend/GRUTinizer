#!/usr/bin/env python2

class Node(object):

    def __init__(self, name, parent=None):
        self.name = name
        self.parent = parent
        self.children = {}

    def AddChild(self, name):
        if '.' in name:
            first,rest = name.split('.', 1)
            self.AddChild(first)
            self.children[first].AddChild(rest)
        else:
            if name not in self.children:
                self.children[name] = Node(name,self)

    def ttk_treeview(self, ttk_tree, parent=''):
        if self.name == 'TNamed':
            return

        if (self.parent is None or
            self.name == 'TDetector' or
            self.parent.name == self.name):
            tree_id = parent
        else:
            tree_id = ttk_tree.insert(parent, 'end', self.full_name,
                                      text = self.name)


        for child in self.children.values():
            child.ttk_treeview(ttk_tree, parent=tree_id)

    def __getitem__(self,name):
        return self.children[name]

    @property
    def full_name(self):
        if self.parent is None:
            return self.name
        elif self.name in [self.parent.name,'TDetector']:
            return self.parent.full_name
        else:
            parent_name = self.parent.full_name
            if parent_name:
                return parent_name + '.' + self.name
            else:
                return self.name
