/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef TESTPACKAGE_TEST8B_H
#define TESTPACKAGE_TEST8B_H

#include "core/opendavinci.h"


#include "core/base/Visitable.h"
#include "core/data/SerializableData.h"


namespace testpackage {
	using namespace std;
	
	class Test8B : public core::data::SerializableData, public core::base::Visitable {
		public:
			Test8B();
	
			Test8B(
			const bool &val0
			);
	
			virtual ~Test8B();
	
			/**
			 * Copy constructor.
			 *
			 * @param obj Reference to an object of this class.
			 */
			Test8B(const Test8B &obj);
	
			/**
			 * Assignment operator.
			 *
			 * @param obj Reference to an object of this class.
			 * @return Reference to this instance.
			 */
			Test8B& operator=(const Test8B &obj);
	
		public:
			/**
			 * @return attribute1.
			 */
			bool getAttribute1() const;
			
			/**
			 * This method sets attribute1.
			 *
			 * @param val Value for attribute1.
			 */
			void setAttribute1(const bool &val);
	
		public:
			/**
			 * This method returns the message id.
			 *
			 * @return Message id.
			 */
			static int32_t ID();
	
			/**
			 * This method returns the short message name.
			 *
			 * @return Short message name.
			 */
			static const string ShortName();
	
			/**
			 * This method returns the long message name include package/sub structure.
			 *
			 * @return Long message name.
			 */
			static const string LongName();
	
			/**
			 * This method returns the message id.
			 *
			 * @return Message id.
			 */
			virtual int32_t getID() const;
	
			/**
			 * This method returns the short message name.
			 *
			 * @return Short message name.
			 */
			virtual const string getShortName() const;
	
			/**
			 * This method returns the long message name include package/sub structure.
			 *
			 * @return Long message name.
			 */
			virtual const string getLongName() const;
	
		public:
			virtual void accept(core::base::Visitor &v);
	
			virtual ostream& operator<<(ostream &out) const;
			virtual istream& operator>>(istream &in);
	
			virtual const string toString() const;
	
		private:
			bool m_attribute1;
	
	};
} // testpackage

#endif /*TESTPACKAGE_TEST8B_H*/
