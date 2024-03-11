export const getSensorTypes = async () => await fetch("/api/sensors/configurable")
    .then(response => response.json());

export const getSensorOptions = async () => await fetch("/api/sensors/options")
    .then(response => response.json());

export const getSensorInfo = async (uuid) => {
    const requests = [
        getSensorOptions(),
        fetch(`/api/sensors/get?uuid=${uuid}`)
            .then(response => response.json()),
    ];

    const results = await Promise.all(requests);

    return {
        options: results[0],
        config: results[1],
    }
}
