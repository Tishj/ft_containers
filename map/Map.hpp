/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Map.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/12/16 15:13:49 by tbruinem      #+#    #+#                 */
/*   Updated: 2021/02/26 11:25:40 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAP_HPP
# define MAP_HPP

# include "GenericFunctions.hpp"
# include <memory>
# include <traits.hpp>

namespace ft
{
	class nodebase
	{
		public:
			nodebase(nodebase *parent = NULL, nodebase *left = NULL, nodebase *right = NULL);
			nodebase(const nodebase& other);
			nodebase& operator = (const nodebase& other);
			~nodebase();
			nodebase	*next();
			nodebase	*prev();
			nodebase	**link_to_parent();

			nodebase	*parent;
			nodebase	*left;
			nodebase	*right;
	};

	template <class Value>
	class node : public nodebase
	{
		public:
			typedef nodebase			base;
			node(const Value& value, nodebase *parent = NULL, nodebase *left = NULL, nodebase *right = NULL) : nodebase(parent, left, right), value(value) {}
			node(const node& other) : nodebase(other.parent, other.left, other.right), value(other.value) {}
			node& operator = (const node& other)
			{
				if (this != &other)
				{
					this->key = other.key;
					this->value = other.value;
					this->parent = other.parent;
					this->left = other.left;
					this->right = other.right;
				}
				return (*this);
			}
			Value&	getval()
			{
				return (this->value);
			}
			~node() {}
		private:
			Value	value;
	};
}

namespace ft
{
	template <class Value, class Reference, class Pointer, class Category = ft::bidirectional_iterator_tag>
	class Iterator
	{
		private:
			typedef ft::nodebase				nodebase;
			typedef ft::node<Value>				node;
		public:
			typedef Category					iterator_category;
			typedef Reference					reference;
			typedef Pointer						pointer;

			Iterator(nodebase *ptr = NULL) : ptr(ptr) {}
			Iterator(const Iterator<Value, Value&, Value*>& other) : ptr(other.ptr) {}
			Iterator&	operator = (const Iterator& other)
			{
				if (this != &other)
				{
					this->ptr = other.ptr;
				}
				return (*this);
			}
			~Iterator() {}
			bool	operator == (const Iterator& other) const
			{
				return (this->ptr == other.ptr);
			}
			bool	operator != (const Iterator& other) const
			{
				return !(*this == other);
			}
			Iterator	operator ++ (int)
			{
				Iterator old(*this);

				this->ptr = this->ptr->next();
				return (old);
			}
			Iterator&	operator ++ ()
			{
				this->ptr = this->ptr->next();
				return (*this);
			}
			Iterator	operator -- (int)
			{
				Iterator old(*this);

				this->ptr = this->ptr->prev();
				return (old);
			}
			Iterator&	operator -- ()
			{
				this->ptr = this->ptr->prev();
				return (*this);
			}
			Pointer	operator -> ()
			{
				return (&static_cast<node *>(this->ptr)->getval());
			}
			Reference	operator * ()
			{
				return (static_cast<node *>(this->ptr)->getval());
			}
			nodebase *getptr() { return this->ptr; }
			nodebase *ptr;
	};
}

namespace ft
{
	template <class Iterator>
	class ReverseIterator
	{
		Iterator	iterator;
		public:
			typedef typename Iterator::iterator_category			iterator_category;
			typedef typename Iterator::reference					reference;
			typedef typename Iterator::pointer						pointer;

			ReverseIterator(ft::nodebase* ptr) : iterator(ptr) {}
			ReverseIterator(const Iterator& other) : iterator(other) {}
			ReverseIterator(const ReverseIterator& other) : iterator(other.iterator) {}
			ReverseIterator& operator = (const ReverseIterator& other)
			{
				this->iterator = other.iterator;
				return (*this);
			}
			ReverseIterator& operator = (const Iterator& other)
			{
				this->iterator = other;
				return (*this);
			}
			~ReverseIterator() {}
			bool				operator == (const ReverseIterator& other) const { return this->iterator == other.iterator; }
			bool				operator != (const ReverseIterator& other) const { return !(*this == other); }
			ReverseIterator&	operator ++ ()
			{
				this->iterator--;
				return (*this);
			}
			ReverseIterator		operator ++ (int)
			{
				ReverseIterator	old(*this);
				++(*this);
				return (old);
			}
			ReverseIterator&	operator -- ()
			{
				this->iterator++;
				return (*this);
			}
			ReverseIterator		operator -- (int)
			{
				ReverseIterator old(*this);
				--(*this);
				return (old);
			}
			reference			operator*()
			{
				return (*iterator);
			}
			pointer				operator->()
			{
				return (&(operator*));
			}
	};
}

namespace ft
{
	template <class Key, class T, class Compare = ft::less<Key>, class Alloc = std::allocator<ft::pair<Key, T> > >
	class map
	{
		public:
			typedef ft::pair<const Key, T>											value_type;
			typedef Key																key_type;
			typedef Compare															key_compare;
			typedef T																mapped_type;
			typedef value_type&														reference;
			typedef value_type*														pointer;
			typedef ft::node<value_type>											mapnode;
			typedef ft::nodebase													mapnodebase;
			typedef Iterator<value_type, value_type&, value_type*>					iterator;
			typedef Iterator<value_type, const value_type&, const value_type*>		const_iterator;
			typedef ReverseIterator<iterator>										reverse_iterator;
			typedef ReverseIterator<const_iterator>									const_reverse_iterator;

			class value_compare : ft::binary_function<value_type,value_type,bool>
			{
				protected:
					Compare comp;
				public:
					value_compare (Compare c) : comp(c) {}
					typedef bool											result_type;
					typedef value_type										first_argument_type;
					typedef value_type										second_argument_type;
					bool operator() (const value_type& x, const value_type& y) const
					{
						return comp(x.first, y.first);
					}
			};

			//CONSTRUCTORS

			explicit map(const Compare& compare = Compare(), const Alloc& alloc = Alloc()) : root(NULL), first(new nodebase()), last(new nodebase()), len(0), compare(compare), allocator(alloc)
			{
				this->first->parent = this->last;
				this->last->parent = this->first;
			}

			map(const map& other) : root(NULL), first(new nodebase()), last(new nodebase()), len(0), compare(other.compare), allocator(other.allocator)
			{
				this->first->parent = this->last;
				this->last->parent = this->first;
				*this = other;
			}

			template <class InputIterator>
			map(InputIterator first, InputIterator last, const Compare& compare = Compare(), const Alloc& alloc = Alloc(),
			typename enable_if<is_iterator<typename InputIterator::iterator_category>::result, InputIterator>::type* = NULL) :
				root(NULL), first(new nodebase()), last(new nodebase()), len(0), compare(compare), allocator(alloc)
			{
				this->first->parent = this->last;
				this->last->parent = this->first;
				this->insert(first, last);
			}

			map& operator = (const map& other)
			{
				if (this != &other)
				{
					this->clear();
					this->insert(other.begin(), other.end());
				}
				return (*this);
			}

			~map()
			{
				this->clear();
				delete this->first;
				delete this->last;
			}

			//ITERATORS

			iterator	begin()
			{
				return (iterator(this->first->parent));
			}

			const_iterator begin() const
			{
				return (const_iterator(this->first->parent));
			}

			iterator	end()
			{
				return (iterator(this->last));
			}

			const_iterator end() const
			{
				return (const_iterator(const_cast<mapnodebase*>(this->last)));
			}

			reverse_iterator rbegin()
			{
				return (reverse_iterator(this->last->parent));
			}

			const_reverse_iterator rbegin() const
			{
				return (const_reverse_iterator(this->last->parent));
			}

			reverse_iterator rend()
			{
				return (reverse_iterator(this->first));
			}

			const_reverse_iterator rend() const
			{
				return (const_reverse_iterator(this->first));
			}

			//CAPACITY

			bool	empty() const
			{
				return (!this->len);
			}

			size_t	size() const
			{
				return (this->len);
			}

			size_t max_size() const
			{
				return (this->allocator.max_size());
			}

			//ELEMENT ACCESS

			T&	operator [] (const Key& key)
			{
				iterator it = find(key);
				if (it == this->end())
					it = insert(value_type(key, T())).first;
				return (it->second);
			}

			//MODIFIERS

			void	clear()
			{
				erase(this->begin(), this->end());
			}

			ft::pair<iterator,bool> insert (const value_type& value)
			{
				iterator it = find(value.first);
				if (it != this->end())
					return (ft::pair<iterator, bool>(it, false));
				mapnodebase **iter = &this->root;
				mapnodebase *parent = NULL;
				while (*iter && *iter != this->first && *iter != this->last)
				{
					parent = (*iter);
					if (this->value_comp()(static_cast<mapnode *>(*iter)->getval(), value))
						iter = &(*iter)->right;
					else if (this->value_comp()(value,static_cast<mapnode *>(*iter)->getval()))
						iter = &(*iter)->left;
				}
				*iter = new mapnode(value, parent, NULL, NULL);
				if (this->last->parent == this->first || this->value_comp()(static_cast<mapnode *>(this->last->parent)->getval(), value))
					connect_parent_child(*iter, &(*iter)->right, this->last);
				if (this->first->parent == this->last || this->value_comp()(value, static_cast<mapnode *>(this->first->parent)->getval()))
					connect_parent_child(*iter, &(*iter)->left, this->first);
				this->len++;
				return (ft::pair<iterator, bool>(iterator(*iter), true));
			}

			template <class InputIterator>
			void insert (InputIterator first, InputIterator last,
				typename enable_if<is_iterator<typename InputIterator::iterator_category>::result, InputIterator>::type* = NULL)
			{
				for (;first != last; first++)
					insert(*first);
			}
			void	erase(iterator it)
			{
				mapnodebase *node = it.getptr();
				if (!node)
					return ;

				mapnodebase *lowest = NULL;
				mapnodebase *highest = NULL;
				if (node->left == this->first)
				{
					lowest = node->next();
					connect_parent_child(lowest, &lowest->left, this->first);
				}
				if (node->right == this->last)
				{
					highest = node->prev();
					connect_parent_child(highest, &highest->right, this->last);
				}
				if (lowest)
					node->left = NULL;
				if (highest)
					node->right = NULL;

				mapnodebase *replacement = NULL;
				if (node->right && node->left)
				{
					replacement = node->left;
					while (replacement->right)
						replacement = replacement->right;
					if (replacement != node->left)
					{
						replacement->parent->right = replacement->left;
						if (replacement->left && replacement->left != this->first)
						{
							replacement->left->parent = replacement->parent;
							replacement->left = NULL;
						}
					}
				}
				else if (node->right)
					replacement = node->right;
				else if (node->left)
					replacement = node->left;

				mapnodebase **replacementparent_link = (replacement) ? replacement->link_to_parent() : NULL;
				if (node->parent && node->parent->left == node)
					node->parent->left = replacement;
				else if (node->parent && node->parent->right == node)
					node->parent->right = replacement;
				else if (!node->parent)
					this->root = replacement;
				if (replacement)
				{
					if (replacementparent_link)
						*replacementparent_link = NULL;
					if (!replacement->left)
						connect_parent_child(replacement, &replacement->left, node->left);
					if (!replacement->right)
						connect_parent_child(replacement, &replacement->right, node->right);
					replacement->parent = node->parent;
				}
				delete static_cast<mapnode *>(node);
				this->len--;
			}

			size_t	erase(const Key& key)
			{
				iterator it = find(key);
				if (it == this->end())
					return (0);
				erase(it);
				return (1);
			}

			void	erase(iterator first, iterator last)
			{
				for (;first != last;)
					erase(first++);
			}

			void	swap(map& other)
			{
				ft::swap(this->allocator, other.allocator);
				ft::swap(this->first, other.first);
				ft::swap(this->last, other.last);
				ft::swap(this->root, other.root);
				ft::swap(this->len, other.len);
				ft::swap(this->compare, other.compare);
			}

			//OBSERVERS

			key_compare key_comp() const
			{
				return (compare);
			}

			value_compare value_comp() const
			{
				return (value_compare(this->compare));
			}

			//OPERATIONS

			iterator find (const key_type& k)
			{
				if (!this->root)
					return (this->end());
				typename mapnode::base *iter = this->root;
				iterator it = iterator(this->root);
				while (iter && iter != this->first && iter != this->last)
				{
					if (this->key_comp()(k, static_cast<mapnode *>(iter)->getval().first))
						iter = iter->left;
					else if (this->key_comp()(static_cast<mapnode *>(iter)->getval().first,k))
						iter = iter->right;
					else
						return (iterator(iter));
				}
				return (this->end());
			}

			const_iterator find (const key_type& k) const
			{
				return (const_iterator(const_cast<map *>(this)->find(k)));
			}

			size_t count (const key_type& k) const
			{
				if (this->find(k) == this->end())
					return (0);
				return (1);
			}

			iterator lower_bound (const key_type& k)
			{
				iterator it = this->find(k);
				if (it == this->end())
					for (it = this->begin(); it != this->end() && this->key_comp()(it->first,k); it++) {}
				else
					--it;
				return (it);
			}

			const_iterator lower_bound (const key_type& k) const
			{
				return (const_iterator(const_cast<map *>(this)->lower_bound(k)));
			}

			iterator upper_bound (const key_type& k)
			{
				iterator it = this->find(k);
				if (it == this->end())
					for (; it != this->begin() && this->key_comp()(k,it->first); it--) {}
				else
					++it;
				return (it);
			}

			const_iterator upper_bound (const key_type& k) const
			{
				return (const_iterator(const_cast<map *>(this)->upper_bound(k)));
			}

			ft::pair<iterator,iterator> equal_range (const key_type& k)
			{
				iterator first = this->lower_bound(k);
				iterator last = this->upper_bound(k);

				if (first != this->end())
					++first;
				return (ft::pair<iterator, iterator>(first, last));
			}

			ft::pair<const_iterator,const_iterator> equal_range (const key_type& k) const
			{
				ft::pair<iterator, iterator>	nonconst = const_cast<map *>(this)->equal_range(k);
				return (ft::pair<const_iterator, const_iterator>(const_iterator(nonconst.first), const_iterator(nonconst.second)));
			}

		private:
			void		connect_parent_child(mapnodebase *parent, mapnodebase **parent_childptr, mapnodebase *child)
			{
				if (child)
					child->parent = parent;
				if ((child == this->first || child == this->last) && (parent == this->first || parent == this->last))
					return ;
				if (parent_childptr)
					*parent_childptr = child;
			}
			mapnodebase	*root;
			mapnodebase	*first;
			mapnodebase	*last;
			size_t	len;
			Compare	compare;
			Alloc	allocator;
	};

	//NON-MEMBER FUNCTIONS

	template<class Key, class T, class Compare, class Alloc>
	bool operator==(const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs)
	{
		if (lhs.size() != rhs.size())
			return (false);
		typename map<Key,T,Compare,Alloc>::value_compare comp = lhs.value_comp();
		typename map<Key,T,Compare,Alloc>::const_iterator it1 = lhs.begin();
		typename map<Key,T,Compare,Alloc>::const_iterator it2 = rhs.begin();
		for (; it1 != lhs.end(); it1++, it2++)
		{
			if (comp(*it1, *it2) || comp(*it2, *it1))
				return (false);
		}
		return (true);
	}

	template<class Key, class T, class Compare, class Alloc>
	bool operator!=(const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class Key, class T, class Compare, class Alloc>
	bool operator<(const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs)
	{
		return (ft::lexicographical_compare(lhs.begin(), rhs.begin(), lhs.end(), rhs.end(), lhs.value_comp()));
	}

	template<class Key, class T, class Compare, class Alloc>
	bool operator>(const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs)
	{
		return (rhs < lhs);
	}

	template<class Key, class T, class Compare, class Alloc>
	bool operator>=(const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs)
	{
		return !(lhs < rhs);
	}

	template<class Key, class T, class Compare, class Alloc>
	bool operator<=(const map<Key,T,Compare,Alloc>& lhs, const map<Key,T,Compare,Alloc>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Key, class T, class Compare, class Alloc>
	void	swap(map<Key,T,Compare,Alloc>& x, map<Key,T,Compare,Alloc>& y)
	{
		x.swap(y);
	}
}

#endif
