// This file is a part of Hologine.
//
// Hologine is a straight-forward framework for interactive simulations,
// most notably video games.
//
// Copyright 2015 Aaron Bolyard.
//
// For licensing information, review the LICENSE file located at the root
// directory of the source package.
#ifndef HOLOGINE_CORE_CONTAINER_INTRUSIVE_LIST_HPP_
#define HOLOGINE_CORE_CONTAINER_INTRUSIVE_LIST_HPP_

#include <boost/iterator/iterator_facade.hpp>

namespace holo
{
	// Defines an intrusive doubly-linked list, as well as nodes in this linked
	// list and operations that can be performed on the list and nodes.
	//
	// Objects that want to make use of the holo::intrusive_list have to have
	// a public-accessible 'next' and 'previous' pointer pair to the type of the
	// node itself.
	//
	// For example, this is a valid node:
	//   struct foo_node
	//   {
	//     foo_node* next;
	//     foo_node* previous;
	//   };
	//
	// On the other hand, this is not:
	//
	//  struct bar_node
	//  {
	//     bar_node* a;
	//     bar_node* b;
	//  }
	namespace intrusive_list
	{
		// An intrusive iterator node.
		template <class Node>
		class iterator :
			public boost::iterator_facade<
				iterator<Node>,
				Node,
				boost::bidirectional_traversal_tag,
				Node*>
		{
			friend class boost::iterator_core_access;

			public:
				// Creates an empty intrusive iterator node.
				iterator();
				
				// Creates an intrusive iterator around the provided node.
				explicit iterator(Node* node);
				
			private:
				// Implementation.
				void increment();
				
				// Implementation.
				void decrement();
				
				// Implementation.
				bool equal(const holo::intrusive_list::iterator<Node>& other) const;
				
				// Implementation.
				Node* dereference() const;
				
				// The node this intrusive list iterator wraps.
				Node* node;
		};

		template <class Node>
		iterator<Node>::iterator() :
			node(nullptr)
		{
			// Nothing.
		}

		template <class Node>
		iterator<Node>::iterator(Node* node) :
			node(node)
		{
			// Nothing.
		}

		template <class Node>
		void iterator<Node>::increment()
		{
			node = node->next;
		}

		template <class Node>
		void iterator<Node>::decrement()
		{
			node = node->previous;
		}

		template <class Node>
		bool iterator<Node>::equal(
			const holo::intrusive_list::iterator<Node>& other) const
		{
			return node == other.node;
		}

		template <class Node>
		Node* iterator<Node>::dereference() const
		{
			return node;
		}
		
		// Creates an iterator from a node.
		//
		// The iterator will be compatible with the standard algorithms, utilities,
		// and so on.
		template <class Node>
		iterator<Node> make_iterator(Node* node)
		{
			return holo::intrusive_list::iterator<Node>(node);
		}
		
		// Creates a const iterator from a node.
		//
		// The iterator will be compatible with the standard algorithms, utilities,
		// and so on.
		template <class Node>
		holo::intrusive_list::iterator<const Node> make_iterator(const Node* node)
		{
			return holo::intrusive_list::iterator<Node>(node);
		}
		
		// Gets the 'end' iterator for a node of the provided class.
		template <class Node>
		iterator<Node> end()
		{
			return holo::intrusive_list::iterator<Node>();
		}
		
		// Removes the provided node from the list.
		//
		// The 'next' and 'previous' pointers of the provided node are untouched by
		// this method.
		//
		// This method should primarily be used if the node is going to be
		// discarded; otherwise, use holo::intrusive_list::unlink(Node*), which
		// updates the node itself to prevent stale references.
		//
		// Returns the removed node.
		template <class Node>
		Node* remove(Node* node)
		{
			Node* previous = node->previous;
			Node* next = node->next;
			
			if (previous != nullptr)
			{
				// Update the 'next' pointer of the previous node.
				previous->next = next;
			}
			
			if (next != nullptr)
			{
				// Update the 'previous' pointer of the next node.
				next->previous = previous;
			}
			
			// Return the removed node.
			return node;
		}
		
		// Removes the provided node from the list and updates the removed node.
		//
		// The 'next' and 'previous' pointers of the provided node are set to NULL.
		//
		// Returns the removed node.
		template <class Node>
		Node* unlink(Node* node)
		{
			remove(node);
			
			// Now clear the next and previous pointers.
			node->next = nullptr;
			node->previous = nullptr;
			
			// ...and return.
			return node;
		}
		
		// Inserts the node into the list before the provided position.
		//
		// This method does not remove the node-to-be-inserted from its current
		// container.
		//
		// Returns the inserted node.
		template <class Node>
		Node* insert_before(Node* node, Node* position)
		{
			// Update the next pointer of 'position->previous'
			Node* previous = position->previous;
			
			if (previous != nullptr)
			{
				previous->next = node;
			}
			
			// Update the previous pointer of 'position'
			position->previous = node;
			
			// And now update the node.
			node->next = position;
			node->previous = previous;
			
			return node;
		}
		
		// Inserts the node into the list after the provided position.
		//
		// This method does not remove the node-to-be-inserted from its current
		// container.
		//
		// Returns the inserted node.
		template <class Node>
		Node* insert_after(Node* node, Node* position)
		{
			// Update the previous pointer of 'position->next'
			Node* next = position->next;
			
			if (next != nullptr)
			{
				position->previous = node;
			}
			
			// Update the next pointer of 'position'
			position->next = node;
			
			// And now update the node.
			node->next = next;
			node->previous = position;
			
			return node;
		}
	}
}

#endif

