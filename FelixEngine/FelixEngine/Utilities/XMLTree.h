//
//  XMLTree.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef XMLTree_h
#define XMLTree_h

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <fstream>
#include <cstdarg>

#include "StringUtils.h"
#include "Vector.h"


namespace fx
{
  /**
   * Provides loading and access the contents of XML Files.
   */
  class XMLTree
  {
  public:
    class Node;
    
    /**
     * Definition of an iterator for Nodes.
     */
    typedef std::list<Node*>::iterator iterator;
    
    /**
     * Definition of a const iterator for Nodes.
     */
    typedef std::list<Node*>::const_iterator const_iterator;
    
    /**
     * Node can be read from and written to an XML file. It can also
     * be used to explore and change an XML structure as well.
     */
    class Node
    {
    public:
      /**
       * Default constructor.
       */
      Node(): _level(0), _parrent(0), _tree(0) {}
      
      /**
       * Construct an Node with an element name.
       * @param element string for the element name.
       */
      Node(const std::string &element): _element(element), _level(0), _parrent(0), _tree(0) {}
      
      /**
       * Construct an Node with an element name and contents.
       * @param element string for the element name.
       * @param contents string for the contents of the new node.
       */
      Node(const std::string &element, const std::string &contents): _element(element), _contents(contents), _level(0), _parrent(0), _tree(0) {}
      
      /**
       * Copy constructor uses assignment operator to copy the given Node.
       * @param node Node to be copied from.
       */
      Node(const Node &node): _level(0), _parrent(0), _tree(0) {*this = node;}
      
      /**
       * Destructor clears the SubNodes.
       */
      ~Node() {clearSubNodes();}
      
      /**
       * Deletes and clears all of the SubNodes.
       */
      void clearSubNodes()
      {
        for (std::list<Node*>::iterator node = _subNodes.begin(); node != _subNodes.end(); ++node)
          delete *node;
        _subNodes.clear();
      }
      
      /**
       * Clears the Attributes.
       */
      void clearAttributes() {_Attributes.clear();}
      
      /**
       * Clears both SubNodes and Attributes.
       */
      void clear()
      {
        clearSubNodes();
        clearAttributes();
      }
      
      /**
       * Adds a valid SubNode to the SubNodes.
       * Also clears the previous parrent.
       * @param [in] node Pointer to an Node object which this takes
       * ownership.
       */
      void addSubNode(Node *node)
      {
        if (node != this)
        {
          if (node->_parrent && node->_parrent != this)
            node->_parrent->removeSubNode(node);
          
          node->_level = 1 + _level;
          node->_parrent = this;
          node->_tree = _tree;
          
          if (std::find(_subNodes.begin(), _subNodes.end(), node) == _subNodes.end())
            _subNodes.push_back(node);
        }
      }
      
      /**
       * Makes a deep copy of the SubNodes of the given node.
       * @param node Node that will have its SubNodes copied from.
       */
      void copySubNodes(const Node &node)
      {
        for (const_iterator itr = node.begin(); itr != node.end(); ++itr)
          addSubNode(new Node(**itr));
      }
      
      /**
       * Copies the Attributes map from the given node.
       * @param node Node that will have its Attributes copied from.
       */
      void copyAttributes(const Node &node)
      {
        _Attributes = node._Attributes;
      }
      
      
      /**
       * Attempts to remove the given node from the SubNodes
       * with out deletion.
       * @param [in,out] node Pointer to an Node that is to be removed.
       */
      void removeSubNode(Node *node)
      {
        std::list<Node*>::iterator itr;
        
        itr = std::find(_subNodes.begin(), _subNodes.end(), node);
        if (itr != _subNodes.end()) {
          node->_level = 0;
          node->_parrent = nullptr;
          node->_tree = nullptr;
          _subNodes.erase(itr);
        }
      }
      
      /**
       * Attempts to delete the first occurence of an Node with
       * the given element name.
       * @param element string for the Elemenent name.
       * @return true if a deletion or false if not.
       */
      bool deleteSubNode(const std::string &element)
      {
        iterator itr = find(element, begin());
        if (itr == end())
          return false;
        
        delete *itr;
        return true;
      }
      
      /**
       * Deletes all of the Nodes with the given element name.
       * @param e string for the Elemenent name.
       */
      void deleteSubNodes(const std::string &e)
      {
        while (deleteSubNode(e))
          ;
      }
      
      /**
       * Sets an Attribute in the Attribute map.
       * @param name string for the name/key of the Attribute.
       * @param value string for the value of the Attribute.
       */
      void setAttribute(const std::string &name, const std::string &value) {_Attributes[name] = value;}
      
      /**
       * Removes an Attribute with the given name.
       * @param name string for the name/key of the Attribute to remove.
       */
      void removeAttribute(const std::string &name) {_Attributes.erase(name);}
      
      
      /**
       * Getter for the number of Nodes that wrap this Node.
       * @return int for the level.
       */
      int level() const {return _level;}
      
      /**
       * Sets the level of this Node and updates is SubNodes.
       * @param level int for the new level.
       */
      void setLevel(int level)
      {
        _level = level;
        for (iterator itr = begin(); itr != end(); ++itr)
          (*itr)->setLevel(_level + 1);
      }
      
      
      /**
       * Assignment operator copies the given Node.
       * @param node Node to be copied from.
       */
      void operator=(const Node &node)
      {
        _element = node._element;
        _contents = node._contents;
        _Attributes = node._Attributes;
        clearSubNodes();
        copySubNodes(node);
      }
      
      
      /**
       * Comparison of Node element with the given string.
       * @param element string to compare with Element name.
       * @return true if the strings match or false if not.
       */
      bool operator==(const std::string &element) const {return _element == element;}
      
      /**
       * Comparison of Node element with the given string.
       * @param element string to compare with Element name.
       * @return false if the strings match or true if not.
       */
      bool operator!=(const std::string &element) const {return _element != element;}
      
      /**
       * Set the contents of this Node.
       * @param contents string for the new contents.
       */
      void setContents(const std::string &contents) {_contents = contents;}
      
      /**
       * Append to the contents of this Node.
       * @param contents string to append to the contents.
       */
      void appendContents(const std::string &contents) {_contents = _contents + contents;}
      
      
      /**
       * Gets the begining iterator for the SubNodes collection.
       * @return iterator to the begining of the SubNodes.
       */
      iterator begin() {return _subNodes.begin();}
      
      /**
       * Gets the ending iterator for the SubNodes collection.
       * @return iterator to the ending of the SubNodes.
       */
      iterator end() {return _subNodes.end();}
      
      
      /**
       * Gets the begining const iterator for the SubNodes collection.
       * @return const iterator to the begining of the SubNodes.
       */
      const_iterator begin() const {return _subNodes.begin();}
      
      /**
       * Gets the const ending iterator for the SubNodes collection.
       * @return const iterator to the ending of the SubNodes.
       */
      const_iterator end() const {return _subNodes.end();}
      
      
      /**
       * Finds the first occurence of an Node with the given Element name.
       * @param element string element name.
       * @return iterator to the found Node or Iterator to the SubNodes
       * end if not found.
       */
      iterator findSubNode(const std::string &element) {return find(element, begin());}
      
      /**
       * Finds the first occurence of an Node with the given Element name
       * after the given iterator position in the SubNodes collection.
       * @param element string element name.
       * @param itr iterator to the starting point of the search.
       * @return iterator to the found Node or iterator to the SubNodes
       * end if not found.
       */
      iterator findSubNode(const std::string &element, iterator itr) {return find(element, itr);}
      
      /**
       * Finds the first occurence of an Node with the given Element name.
       * @param element string element name.
       * @return const iterator to the found Node or Iterator to the SubNodes
       * end if not found.
       */
      const_iterator findSubNode(const std::string &element) const {return find(element, begin());}
      
      /**
       * Finds the first occurence of an Node with the given Element name
       * after the given iterator position in the SubNodes collection.
       * @param element string element name.
       * @param itr iterator to the starting point of the search.
       * @return const iterator to the found Node or iterator to the SubNodes
       * end if not found.
       */
      const_iterator findSubNode(const std::string &element, const_iterator itr) const {return find(element, itr);}
      
      
      /**
       * Gets the Element name of this Node.
       * @return string for the Element name.
       */
      std::string element() const {return _element;}
      
      /**
       * Gets the Contents of this Node.
       * @return string for the Contents.
       */
      std::string contents() const {return _contents;}
      
      /**
       * Gets the Attribute value for the given name/key.
       * @param name string for the name/key of the attribute.
       * @return string for the value or an empty string if not found.
       */
      std::string attribute(const std::string &name) const
      {
        std::map<std::string, std::string>::const_iterator itr = _Attributes.find(name);
        return itr != _Attributes.end() ? itr->second : "";
      }
      
      /**
       * Gets the boolean value of the Attribute for the given name/key.
       * @param name string for the name/key of the attribute.
       * @return bool value of the string.
       */
      bool attributeAsBoolean(const std::string &name) const
      {
        std::string str = attribute(name);
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str == "true";
      }
      
      /**
       * Gets the Int value of the Attribute for the given name/key.
       * @param name string for the name/key of the attribute.
       * @return int value of the string or 0 if the attribute is not found or miss-read.
       */
      int attributeAsInt(const std::string &name) const
      {
        int value = 0;
        fx::StringUtils::parseInt(value, attribute(name));
        return value;
      }
      
      /**
       * Gets the float value of the Attribute for the given name/key.
       * @param name string for the name/key of the attribute.
       * @return float value of the string or 0.0f if the attribute is not found or miss-read.
       */
      float attributeAsFloat(const std::string &name) const
      {
        float value = 0;
        fx::StringUtils::parseFloat(value, attribute(name));
        return value;
      }
      
      /**
       * Attempts to get the Contents of the first SubNode with the given
       * Element name.
       * @param element string for the Element name to search for.
       * @return string of the contents of the found SubNode or empty
       * empty string if not found.
       */
      std::string subContents(const std::string &element) const
      {
        const_iterator itr = findSubNode(element);
        return itr != end() ? (*itr)->contents() : "";
      }
      
      /**
       * Gets the first occurence of a SubNode with the given Element name.
       * If not available, creates a new sub node with the given Element name.
       * @param element string for the element name to search for.
       * @return pointer to the found Node.
       */
      Node* subNode(const std::string &element)
      {
        if (!hasSubNode(element))
          addSubNode(new Node(element));
        return *findSubNode(element);
      }
      
      /**
       * Gets the first occurence of a SubNode with the given Element name.
       * @param element string for the element name to search for.
       * @return const pointer to the found Node or NULL if not found.
       */
      const Node* subNode(const std::string &element) const
      {
        const_iterator itr = findSubNode(element);
        return itr != end() ? *itr : NULL;
      }
      
      /**
       * Checks if an Attribute is present in the Attribute Map.
       * @param attribute string attribute value to check for.
       * @return true if the Attribute is in the Attribute Map or false if not.
       */
      bool hasAttribute(const std::string &attribute) const {return _Attributes.find(attribute) != _Attributes.end();}
      
      template <typename T>
      bool hasAttributes(T arg) const {
        return hasAttribute(arg);
      }
      
      template <typename T, typename... R>
      bool hasAttributes(T arg, R... rest) const {
        return hasAttribute(arg) && hasAttributes(rest...);
      }
      
      /**
       * Checks if the SubNodes collection contains an Node with the given element
       * name.
       * @param element string for the Element name to search for.
       * @return true if there is a SubNode with the Element name or false if not.
       */
      bool hasSubNode(const std::string &element) const {return findSubNode(element) != end();}
      
      /**
       * Gets the number of SubNodes in the Node.
       * @return number of SubNodes.
       */
      size_t numberSubNodes() const {return _subNodes.size();}
      
      /**
       * Gets the number of Attributes in the Node.
       * @return number of Attributes.
       */
      size_t numberAttributes() const {return _Attributes.size();}
      
      /**
       * Gets a pointer to the Tree that owns this node.
       * @return pointer to an XMLTree or Null if not in a tree.
       */
      XMLTree *tree() const {return _tree;}
      
      /**
       * Write an Node and its SubNodes to the given output stream.
       * @param os output stream to be written to.
       * @param node Node that will write to the output stream.
       * @return output stream that was written to by the Node.
       */
      friend std::ostream &operator<<(std::ostream &os, const Node &node) {return node.write(os);}
      
      /**
       * Read into an Node and its SubNodes from the given input stream.
       * @param is input stream to be read from.
       * @param node Node that will read from the input stream.
       * @return input stream that was read from by the Node.
       */
      friend std::istream &operator>>(std::istream &is, Node &node) {return node.read(is);}
      
    private:
      friend XMLTree;
      
      /**
       * Finds the first occurence of an Node with the given Element name
       * after the given iterator position in the SubNodes collection.
       * @param element string element name.
       * @param itr iterator to the starting point of the search.
       * @return iterator to the found Node or iterator to the SubNodes
       * end if not found.
       */
      iterator find(const std::string &element, iterator itr)
      {
        while (itr != end() && **itr != element)
          ++itr;
        return itr;
      }
      
      /**
       * Finds the first occurence of an Node with the given Element name
       * after the given iterator position in the SubNodes collection.
       * @param element string element name.
       * @param itr iterator to the starting point of the search.
       * @return const iterator to the found Node or iterator to the SubNodes
       * end if not found.
       */
      const_iterator find(const std::string &element, const_iterator itr) const
      {
        while (itr != end() && **itr != element)
          ++itr;
        return itr;
      }
      
      
      /**
       * Writes this Node and SubNodes to the given output stream.
       * @param os output stream to be written to.
       * @return output stream that was written to.
       */
      std::ostream &write(std::ostream &os) const
      {
        std::map<std::string, std::string>::const_iterator attItr;
        
        /* write the Element name in the opening node. */
        os << std::string(_level*2, ' ') << "<" << _element;
        
        /* write the Attributes in the opening node. */
        for (attItr = _Attributes.begin(); attItr != _Attributes.end(); ++attItr)
          os << " " << attItr->first << "=\"" << attItr->second << "\"";
        
        if (_subNodes.size())
        {
          /* Recursively write the SubNodes between the opening an closing tags
           * assuming an empty Contents string. */
          os << ">" << std::endl;
          for (const_iterator sub = begin(); sub != end(); ++sub)
            os << **sub;
          os << std::string(_level*2, ' ') << "</" << _element << ">" << std::endl;
        }
        else if (_contents == "")
        {
          /* close to a single tag if no Contents. */
          os << " />" << std::endl;
        }
        else
        {
          /* write the Contents between opening and closing tags */
          os << ">" << _contents << "</" << _element << ">" << std::endl;
        }
        
        return os;
      }
      
      /**
       * Reads the given input stream to this Node and SubNodes.
       * @param is input stream to read from.
       * @return input stream that was read from.
       */
      std::istream &read(std::istream &is)
      {
        bool commented = false;
        std::list<Node *> stack;
        std::string line;
        stack.push_back(this);
        
        while (!is.eof() && stack.size())
        {
          getline(is, line);
          RemoveComments(line, commented);
          if (line != "")
            ParseLine(&stack, line);
        }
        
        return is;
      }
      
      /**
       * Removes the Comments out of a line
       * @param [in, out] line to edit comments out.
       * @param [in, out] commented state to determine if the line is initally commented out.
       */
      static void RemoveComments(std::string &line, bool &commented)
      {
        std::string::size_type start = 0;
        std::string::size_type end = 0;
        do {
          if (commented)
          {
            end = line.find("-->");
            if (end != std::string::npos)
            {
              line = line.substr(end+3, line.size());
              commented = false;
            }
            else
              line = line.substr(0, start);
          }
          else
          {
            start = line.find("<!--");
            if (start != std::string::npos)
            {
              end = line.find("-->", start);
              if (end != std::string::npos)
                line = line.substr(0, start) + line.substr(end+3, line.size());
              else
              {
                line = line.substr(0, start);
                commented = true;
              }
            }
          }
          
        } while (start != std::string::npos && end != std::string::npos);
      }
      
      /**
       * Parses a line for either adding to or removing tags from the stack.
       * @param [in, out] stack pointer to the stack of parrent Nodes.
       * @param line string of the line to be parsed.
       */
      static void ParseLine(std::list<Node *> *stack, const std::string &line)
      {
        std::string::size_type loc1, loc2;
        
        loc1 = line.find("<");
        loc2 = line.find(">");
        
        if (loc1 != std::string::npos && loc2 != std::string::npos)
        {
          /* Found either a new or closing tag. */
          ++loc1;
          ParseTag(stack, line.substr(loc1, loc2 - loc1), line.substr(loc2+1));
        }
        else
        {
          /* Check for any Contents */
          loc1 = line.find_first_not_of("\t ");
          if (loc1 != std::string::npos && stack->size())
            stack->back()->appendContents("\n" + line);
        }
      }
      
      /**
       * Attempts to parse a tag from the set of strings.
       * @param [in, out] stack pointer to the stack of parrent Nodes.
       * @param str1 string potentially holding an Element name and Attributes.
       * @param str2 string potentially holding Conents or SubNodes.
       */
      static void ParseTag(std::list<Node *> *stack, const std::string &str1, const std::string &str2)
      {
        std::string::size_type loc;
        
        /* Check for a close. */
        loc = str1.find("/");
        
        if (loc == std::string::npos)
        {
          /* No close */
          if (!stack->size())
            stack->push_back(TagFromStr(str1));
          else
          {
            Node *subNode = TagFromStr(str1);
            stack->back()->addSubNode(subNode);
            stack->push_back(subNode);
          }
          
          /* Check for Contents */
          if (str2 != "")
          {
            loc = str2.find("</");
            if (loc != std::string::npos)
            {
              stack->back()->appendContents(str2.substr(0, loc));
              if (stack->size() > 1)
                stack->pop_back();
            }
            else
              stack->back()->appendContents(str2);
          }
        }
        else if (loc == 0)
        {
          /* Closing tag */
          if (stack->size() > 1)
            stack->pop_back();
        }
        else
        {
          /* Self closing tag */
          stack->back()->addSubNode(TagFromStr(str1));
        }
      }
      
      /**
       * Creates an Node from a string for its Element and potential Attributes.
       * @param str string holding an Element name and potential Attributes.
       */
      static Node* TagFromStr(const std::string &str)
      {
        std::string subStr = str;
        std::string::size_type loc = str.find_first_of(" /");
        Node *node = new Node(str.substr(0, loc));
        
        /* Attempt to parse any Attributes */
        loc = str.find_first_not_of(" ", loc);
        while (loc != std::string::npos && subStr[loc] != '/')
        {
          std::string name;
          subStr = subStr.substr(loc);
          
          loc = subStr.find("=\"");
          if (loc == std::string::npos)
            break;
          name = subStr.substr(0, loc);
          
          subStr = subStr.substr(loc+2);
          loc = subStr.find("\"");
          if (loc == std::string::npos)
            break;
          node->setAttribute(name, subStr.substr(0, loc));
          
          loc = subStr.find_first_not_of(" ", loc+1);
        }
        
        return node;
      }
      
      int         _level;    /**< Level of the Node */
      Node       *_parrent;  /**< Parrent Node or NULL if no Parrent */
      std::string _element;  /**< Element name */
      std::string _contents; /**< String Contents */
      XMLTree    *_tree;     /**< the tree containing this node */
      
      std::list<Node*> _subNodes; /**< Collection of SubNodes */
      std::map<std::string, std::string> _Attributes; /**< Map of Attributes */
    };
    
    
    XMLTree(): _rootNode("XMLTree")
    {
      _rootNode._tree = this;
      _rootNode.setLevel(-1);
    }
    ~XMLTree() {}
    
    /**
     * Gets the number of Base Nodes in the Tree.
     * @return number of Base Nodes.
     */
    size_t numberBaseNodes() const {return _rootNode.numberSubNodes();}
    
    /**
     * Checks if the Tree is empty of Base Nodes.
     * @return true if empty or false otherwise.
     */
    bool isEmpty() const {return _rootNode.numberSubNodes() == 0;}
    
    /**
     * Gets the begining iterator for the SubNodes in the Root Node.
     * @return iterator to the begining of the Root Node's SubNodes.
     */
    iterator begin() {return _rootNode.begin();}
    
    /**
     * Gets the ending iterator for the SubNodes in the Root Node.
     * @return iterator to the ending of the RootNode's SubNodes.
     */
    iterator end() {return _rootNode.end();}
    
    /**
     * Gets the begining iterator for the SubNodes in the Root Node.
     * @return iterator to the begining of the Root Node's SubNodes.
     */
    const_iterator begin() const {return _rootNode.begin();}
    
    /**
     * Gets the ending iterator for the SubNodes in the Root Node.
     * @return iterator to the ending of the RootNode's SubNodes.
     */
    const_iterator end() const {return _rootNode.end();}
    
    /**
     * Write the Root Node and its SubNodes to the given output stream.
     * @param os output stream to be written to.
     * @param tree XMLTree that will write to the output stream.
     * @return output stream that was written to by the Node.
     */
    friend std::ostream &operator<<(std::ostream &os, const XMLTree &tree)
    {
      std::ostream *ptr = &os;
      for (const_iterator itr = tree.begin(); itr != tree.end(); ++itr)
        ptr = &(*ptr << **itr);
      return *ptr;
    }
    
    /**
     * Loads the contents of the given file to the XMLTree.
     * @param filePath to the file to read from.
     * @return true if successfully read or false if unable to read from file.
     */
    bool loadFile(const std::string &filePath)
    {
      std::fstream fileIn;
      std::string line;
      
      size_t pos = filePath.find_last_of('/');
      if (pos != std::string::npos)
      {
        _path = filePath.substr(0, pos+1);
        _url = "file://" + _path;
      }
      
      fileIn.open(filePath.c_str(), std::ios::in);
      if (fileIn.is_open())
      {
        _rootNode.clearSubNodes();
        getline(fileIn, line);
        
        fileIn >> _rootNode;
        fileIn.close();
        return true;
      }
      
      std::cerr << "error reading XML file: " << filePath << std::endl;
      return false;
    }
    void setUrl(const std::string &url) {_url = url;}
    
    std::string url() const {return _url;}
    std::string path() const {return _path;}
    
  private:
    Node _rootNode;
    std::string _url;
    std::string _path;
  };
}

#endif /* XMLTree_h */
