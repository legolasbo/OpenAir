const connectors = {
    x4: "X4",
    x6: "X6",
}
const configurableSensors = document.createElement("ul");

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
    const jsonOptions = await fetchJson(`/api/sensors/options?uuid=${uuid}`);
    const jsonConfig = await fetchJson(`/api/sensors/get?uuid=${uuid}`);

    contentElement.innerHTML = "";
    contentElement.append(createElement("h3", {}, `Configure ${jsonConfig.name}`));

    const form = createElement("form", {action:"/api/sensors/configure", method: "post"});
    contentElement.append(form);

    for (const [name, info] of Object.entries(jsonOptions)) {
        if (info.type !== "hidden") {
            form.append(createElement("h4", {}, info.label));
        }

        switch (info.type) {
            case "select": 
                form.append(createRadioButtons(name, info.options, "", jsonConfig[name]));
                break;
            case "hidden":
                form.append(createHiddenInput(name, jsonConfig[name]));
                break;
            case "text":
                form.append(createTextInput(name, jsonConfig[name]));
                break;
        }
    }

    form.append(createSubmitButton("Submit"));
}

function createEditSensorButton(uuid) {
    const button = createElement("button", {type: "button"}, "Edit");
    button.onclick = () => {configureSensorScreen(uuid)};
    return button;
}

function createAddSensorForm(sensorType, options) {
    const wrapper = document.createElement("div");
    wrapper.append(options.name);
    const form = createElement(
        "form",
        {action: "/api/sensors/add", method: "post"},
        createHiddenInput("sensorType", sensorType),
        createRadioButtons("connectionType", options.connections, sensorType, true),
        createRadioButtons("connection", connectors, sensorType, true),
        createSubmitButton("add")
    )
    wrapper.append(form);

    return wrapper;
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
    wrapper.innerHTML = "<thead><tr><td>Name</td><td>connector</td><td>connection</td><td>Type</td><td></td></thead>";
    const tbody = document.createElement("tbody");
    wrapper.appendChild(tbody);

    for (const sensor of Object.values(sensors)) {
        const tr = document.createElement("tr");
        tbody.appendChild(tr);

        tr.appendChild(createElement("td", {}, sensor.name ? sensor.name : "Unnamed"));
        tr.appendChild(createElement("td", {}, sensor.connector));
        tr.appendChild(createElement("td", {}, sensor.connection));
        tr.appendChild(createElement("td", {}, sensor.type));
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
