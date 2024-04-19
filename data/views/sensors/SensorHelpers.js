export const getSensorTypes = async () => await fetch("/api/sensors/types")
    .then(response => response.json());

export const getSensorOptions = async (uuid) => await fetch(`/api/sensors/options?uuid=${uuid}`)
    .then(response => response.json());

export const getOptionsForSensorType = async (type) => await fetch(`/api/sensors/options?type=${type}`)
    .then(response => response.json());

export const getSensorInfo = async (uuid) => {
    const requests = [
        getSensorOptions(uuid),
        fetch(`/api/sensors/get?uuid=${uuid}`)
            .then(response => response.json()),
    ];

    const results = await Promise.all(requests);

    return {
        options: results[0],
        config: results[1],
    }
}

export const getSensorDetails = async (uuid) => await fetch(`/api/sensors/details?uuid=${uuid}`)
    .then(response => response.json());
