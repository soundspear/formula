/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_EVENTS_EVENTHUB_INCLUDED
#define FORMULA_EVENTS_EVENTHUB_INCLUDED

#include <optional>
#include <boost/signals2.hpp>
#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventType.hpp>

namespace formula::events {
	class EventHub
	{
	public:
		EventHub() = default;

		void publish(EventType eventType) {
            if (events.contains(eventType)) {
                events[eventType](boost::any());
            }
        }

		template <typename T>
		void publish(EventType eventType, T eventArgs)
		{
			if (events.contains(eventType)) {
				boost::any anyArgs = eventArgs;
				events[eventType](anyArgs);
			}
		}

        template <typename Type>
		void subscribe(EventType eventType, boost::function<void(boost::any)> callback, Type* owner) {
            callbacks.push_back(std::move(callback));
            if (!events.contains(eventType)) {
                events[eventType] = boost::signals2::signal<void(boost::any)>();
            }

            connections[owner] = events[eventType].connect(callbacks.back());
        }

        template <typename ComponentType>
        void subscribeOnUiThread(EventType eventType,
                                 boost::function<void(boost::any, ComponentType*)> callback,
                                 ComponentType* owner) {
            if (!events.contains(eventType)) {
                events[eventType] = boost::signals2::signal<void(boost::any)>();
            }

            auto sp = juce::Component::SafePointer<ComponentType> (owner);
            callbacks.push_back([=] (boost::any arg) {
                if (sp == nullptr) return;
                MessageManager::callAsync([=] {
                    callback(arg, sp.getComponent());
                });
            });
            connections[owner] = events[eventType].connect(callbacks.back());
        }

        template <typename Type>
        void unsubscribe(Type* owner) {
            if (connections.contains((void*)owner)) {
                connections[(void*)owner].disconnect();
            }
        }

	private:
		std::map<EventType, boost::signals2::signal<void(boost::any)>> events;
        std::vector<boost::function<void(boost::any)>> callbacks;
        std::map<void*, boost::signals2::connection> connections;
	};
}

#endif // FORMULA_EVENTS_EVENTHUB_INCLUDED