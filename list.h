/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef LIST_H
#define LIST_H

template <typename T> class List
{
private:
	T * m_pItems;
	int m_nCount;
	int m_nBufferSize;
public:
	List<T>() {
		this->m_pItems = new T[20];
		this->m_nCount = 0;
		this->m_nBufferSize = 20;
	}
	
	virtual ~List() {
		delete [] this->m_pItems;
	}
	
	T * ptr() {
		return m_pItems;
	}
	
	int Count() {
		return m_nCount;
	}
	
private:
	void Realloc() {
		int nNewBufferSize = m_nBufferSize * 2;
		T * items = new T[nNewBufferSize];
		
		memcpy(items, m_pItems, sizeof(T *) * m_nCount);
		
		delete [] m_pItems;
		
		m_pItems = items;
		m_nBufferSize = nNewBufferSize;
	}
	
public:
	void Add(T item) {
		if (m_nBufferSize - m_nCount == 0) {
			Realloc();
		}
		
		m_pItems[m_nCount++] = item;
	}
	
	void RemoveAt(int index) {
		for (int i = index; i < m_nCount; i++) {
			m_pItems[i] = m_pItems[i + 1];
		}
		
		m_nCount--;
	}
};

#endif // LIST_H
