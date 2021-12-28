#ifndef FORMULA_EVENTS_EVENT_HUB
#define FORMULA_EVENTS_EVENT_HUB

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

		void subscribe(EventType eventType, boost::function<void(boost::any)> callback) {
            if (!events.contains(eventType)) {
                events[eventType] = boost::signals2::signal<void(boost::any)>();
            }
            events[eventType].connect(callback);
        }

        template <typename ComponentType>
        void subscribeOnUiThread(EventType eventType,
                                 boost::function<void(boost::any, ComponentType*)> callback,
                                 ComponentType* componentToUse) {
            if (!events.contains(eventType)) {
                events[eventType] = boost::signals2::signal<void(boost::any)>();
            }

            auto sp = juce::Component::SafePointer<ComponentType> (componentToUse);
            events[eventType].connect([=] (boost::any arg) {
                if (sp == nullptr) return;
                MessageManager::callAsync([=] {
                    callback(arg, sp.getComponent());
                });
            });
        }
	private:
		std::map<EventType, boost::signals2::signal<void(boost::any)>> events;
	};
}

#endif // FORMULA_EVENTS_EVENT_HUB