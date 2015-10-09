#!/usr/bin/env python2

import re

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

    def ttk_treeview(self, ttk_tree, parent='', lookup_table=None):
        if self.name == 'TNamed':
            return

        if (self.parent is None or
            self.name == 'TDetector' or
            self.parent.name == self.name):
            tree_id = parent
        else:
            unique_name = self.full_name(unique=True)
            tree_id = ttk_tree.insert(parent, 'end', unique_name,
                                      text = self.name)
            if lookup_table is not None and not self.children:
                param_name = self.full_name(unique=False)
                lookup_table[unique_name] = param_name

        for child in self.children.values():
            child.ttk_treeview(ttk_tree, parent=tree_id, lookup_table=lookup_table)

    def __getitem__(self,name):
        return self.children[name]

    def full_name(self, unique=True):
        # Get the name
        if self.parent is None:
            output = self.name if unique else ''
        elif self.name in [self.parent.name,'TDetector']:
            output =  self.parent.full_name(unique)
        else:
            parent_name = self.parent.full_name(unique)
            if parent_name:
                output = parent_name + '.' + self.name
            else:
                output = self.name

        # If the name ends in [num], strip it
        output = re.sub(r'\[[0-9]*\]$','',output)
        return output
