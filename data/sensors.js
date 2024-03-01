
async function sensorsScreen() {
    const configurableSensors = await getConfigurableSensors();
    const sensors = await loadSensors();

    contentElement.innerHTML = "";
    contentElement.appendChild(createElement("h3", {}, "Configured sensors"));
    contentElement.appendChild(sensors);

    contentElement.appendChild(createElement("h3", {}, "Add sensor"));
    contentElement.appendChild(configurableSensors);
}

async function configureSensorScreen(uuid) {
    contentElement.innerHTML = uuid;
}

function createEditSensorButton(uuid) {
    const button = createElement("button", {type: "button"}, "Edit");
    button.onclick = () => {configureSensorScreen(uuid)};
    return button;
}

const createDeleteSensorButton = (uuid) => createElement(
    "form",
    {
        action: "/api/sensors/delete",
        method: "post",
    }, 
    createHiddenInput("uuid", uuid),
    createSubmitButton("delete")
);

async function loadSensors() {
    const sensors = await fetchJson("/api/sensors");
    const wrapper = document.createElement("table");
    wrapper.innerHTML = "<thead><tr><td>Type</td><td>connector</td><td>connection</td><td></td></thead>";
    const tbody = document.createElement("tbody");
    wrapper.appendChild(tbody);

    for (const sensor of Object.values(sensors)) {
        const tr = document.createElement("tr");
        tbody.appendChild(tr);

        tr.appendChild(createElement("td", {}, sensor.type));
        tr.appendChild(createElement("td", {}, sensor.connector));
        tr.appendChild(createElement("td", {}, sensor.connection));
        const td = createElement("td", {});
        td.append(createEditSensorButton(sensor.uuid));
        td.append(createDeleteSensorButton(sensor.uuid));
        tr.appendChild(td);
    }

    return wrapper;
}

async function getConfigurableSensors() {
    if (configurableSensors.childNodes.length > 0) return configurableSensors;

    const sensorOptions = await fetchJson("/api/sensors/configurable");

    for (const [sensorType, options] of Object.entries(sensorOptions)) {
        const item = document.createElement("li");
        item.appendChild(createAddSensorForm(sensorType, options));
        configurableSensors.appendChild(item);
    }
    
    return configurableSensors;
}
