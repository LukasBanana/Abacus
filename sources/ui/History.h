/*
 * History.h
 * 
 * This file is part of the "Abacus" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __AC_HISTORY_H__
#define __AC_HISTORY_H__


#include <vector>


template <typename T, std::size_t N = 100u>
class History
{

    public:
        
        void Add(const T& value)
        {
            /* Add entry if this was not the previous one */
            if (values_.empty() || values_.back() != value)
                values_.push_back(value);

            /* Get index to last entry */
            idx_ = values_.size() - 1;

            /* Remove first element if list is too long */
            if (values_.size() > N)
                values_.erase(values_.begin());
        }

        bool Get(T& value)
        {
            if (!values_.empty())
            {
                value = values_[idx_];
                return true;
            }
            return false;
        }

        void Prev()
        {
            if (idx_ > 0)
                --idx_;
        }

        void Next()
        {
            if (idx_ + 1 < values_.size())
                ++idx_;
        }

        bool IsBegin() const
        {
            return idx_ == 0;
        }

        bool IsEnd() const
        {
            return idx_ + 1 == values_.size();
        }

    private:
        
        std::vector<T>  values_;
        std::size_t     idx_ = 0;

};


#endif



// ================================================================================