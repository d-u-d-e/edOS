#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <os/include/sys/init.h>

/**
 * @brief Runtime device dynamic structure (in RAM) per driver instance
 *
 * Fields in this are expected to be default-initialized to zero. The
 * kernel driver infrastructure and driver access functions are
 * responsible for ensuring that any non-zero initialization is done
 * before they are accessed.
 */
struct device_state {
	/**
	 * Device initialization return code (positive errno value).
	 *
	 * Device initialization functions return a negative errno code if they
	 * fail. In Zephyr, errno values do not exceed 255, so we can store the
	 * positive result value in a uint8_t type.
	 */
	uint8_t init_res;

	/** Indicates the device initialization function has been
	 * invoked.
	 */
	bool initialized : 1;
};

/**
 * @brief Runtime device structure (in ROM) per driver instance
 */
struct device {
	/** Name of the device instance */
	const char *name;
	/** Address of device instance config information */
	const void *config;
	/** Address of the API structure exposed by the device instance */
	const void *api;
	/** Address of the common device state */
	struct device_state *state;
	/** Address of the device instance private data */
	void *data;
};

/**
 * @brief Initializer for @ref device.
 *
 * @param name_ Name of the device.
 * @param data_ Reference to device data.
 * @param config_ Reference to device config.
 * @param api_ Reference to device API ops.
 * @param state_ Reference to device state.
 */
#define Z_DEVICE_INIT(name_, data_, config_, api_, state_)         \
	{                                                                      \
		.name = name_,                                                 \
		.config = (config_),                                           \
		.api = (api_),                                                 \
		.state = (state_),                                             \
		.data = (data_)                                               \
	}

/**
 * @brief Expands to the name of a global device object.
 *
 * Return the full name of a device object symbol created by DEVICE_DEFINE(),
 * using the `dev_id` provided to DEVICE_DEFINE(). 
 *
 * @param dev_id Device identifier.
 *
 * @return The full name of the device object defined by device definition
 * macros.
 */
#define DEVICE_NAME_GET(dev_id) _CONCAT(__device_, dev_id)

/**
 * @brief Device section name (used for sorting purposes).
 *
 * @param level Initialization level
 * @param prio Initialization priority
 */
#define Z_DEVICE_SECTION_NAME(level, prio)                                     \
	_CONCAT(INIT_LEVEL_ORD(level), _##prio)


#define Z_DEVICE_BASE_DEFINE(dev_id, name, data, config, level, prio, api, state) \                        \
	static const STRUCT_SECTION_ITERABLE_NAMED(device,                            \
		Z_DEVICE_SECTION_NAME(level, prio),                            \
		DEVICE_NAME_GET(dev_id)) =                                     \
		Z_DEVICE_INIT(name, data, config, api, state)


/* deprecated device initialization levels */
#define Z_DEVICE_LEVEL_DEPRECATED_EARLY                                        \
	__WARN("EARLY device driver level is deprecated")
#define Z_DEVICE_LEVEL_DEPRECATED_PRE_KERNEL_1
#define Z_DEVICE_LEVEL_DEPRECATED_PRE_KERNEL_2
#define Z_DEVICE_LEVEL_DEPRECATED_POST_KERNEL
#define Z_DEVICE_LEVEL_DEPRECATED_APPLICATION                                  \
	__WARN("APPLICATION device driver level is deprecated")
#define Z_DEVICE_LEVEL_DEPRECATED_SMP                                          \
	__WARN("SMP device driver level is deprecated")

/**
 * @brief Issue a warning if the given init level is deprecated.
 *
 * @param level Init level
 */
#define Z_DEVICE_LEVEL_CHECK_DEPRECATED_LEVEL(level)                           \
	Z_DEVICE_LEVEL_DEPRECATED_##level

/**
 * @brief Maximum device name length.
 *
 * The maximum length is set so that device_get_binding() can be used from
 * userspace.
 */
#define Z_DEVICE_MAX_NAME_LEN 48U

/**
 * @brief Compile time check for device name length
 *
 * @param name Device name.
 */
#define Z_DEVICE_NAME_CHECK(name)                                              \
	BUILD_ASSERT(sizeof(Z_STRINGIFY(name)) <= Z_DEVICE_MAX_NAME_LEN,       \
			    Z_STRINGIFY(DEVICE_NAME_GET(name)) " too long")

/**
 * @brief Define the init entry for a device.
 *
 * @param dev_id Device identifier.
 * @param init_fn_ Device init function.
 * @param level Initialization level.
 * @param prio Initialization priority.
 */
#define Z_DEVICE_INIT_ENTRY_DEFINE(dev_id, init_fn_, level, prio)     \
	Z_DEVICE_LEVEL_CHECK_DEPRECATED_LEVEL(level)                           \
                                                                               \
	static const Z_DECL_ALIGN(struct init_entry) __used __noasan           \
		Z_INIT_ENTRY_SECTION(level, prio, 0)          \
		Z_INIT_ENTRY_NAME(DEVICE_NAME_GET(dev_id)) = {                 \
			.init_fn = {.dev = (init_fn_)},                        \
			.dev = &DEVICE_NAME_GET(dev_id),                       \
	}


/**
 * @brief Define a @ref device and all other required objects.
 *
 * This is the common macro used to define @ref device objects.
 *
 * @param dev_id Device identifier (used to name the defined @ref device).
 * @param name Name of the device.
 * @param init_fn Device init function.
 * @param data Reference to device data.
 * @param config Reference to device config.
 * @param level Initialization level.
 * @param prio Initialization priority.
 * @param api Reference to device API.
 * @param state Reference to device state.
 */
#define Z_DEVICE_DEFINE(dev_id, name, init_fn, data, config,      \
			level, prio, api, state)                          \
	Z_DEVICE_NAME_CHECK(name);                                             \
                                                                               \
	Z_DEVICE_BASE_DEFINE(dev_id, name, data, config, level,   \
			     prio, api, state);    \
                                                                               \
	Z_DEVICE_INIT_ENTRY_DEFINE(dev_id, init_fn, level, prio)
