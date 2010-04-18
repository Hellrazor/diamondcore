/*
 * Copyright (C) 2010 DiamondCore <http://diamondcore.eu/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DIAMOND_SINGLETON_H
#define DIAMOND_SINGLETON_H

/**
 * @brief class Singleton
 */

#include "CreationPolicy.h"
#include "ThreadingModel.h"
#include "ObjectLifeTime.h"

namespace Diamond
{
    template
        <
        typename T,
        class ThreadingModel = Diamond::SingleThreaded<T>,
        class CreatePolicy = Diamond::OperatorNew<T>,
        class LifeTimePolicy = Diamond::ObjectLifeTime<T>
        >
        class DIAMOND_DLL_DECL Singleton
    {
        public:
            static T& Instance();

        protected:
            Singleton() {};

        private:

            // Prohibited actions...this does not prevent hijacking.
            Singleton(const Singleton &);
            Singleton& operator=(const Singleton &);

            // Singleton Helpers
            static void DestroySingleton();

            // data structure
            typedef typename ThreadingModel::Lock Guard;
            static T *si_instance;
            static bool si_destroyed;
    };
}
#endif
