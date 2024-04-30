import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";
import { getSensorOptions, getSensorTypes } from "./SensorHelpers.js";

export default class extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle(`Edit sensor: ${this.params.uuid}`);
    }

    async getHtml() {
        const options = await getSensorOptions(this.params.uuid);
        const sensorTypes = await getSensorTypes();
        const sensorType = sensorTypes[options.type.value];
        
        this.setTitle(`Edit ${sensorType} sensor`);

        return `
            <h1>Edit ${sensorType} sensor</h1>
            ${formFromOptions("/api/sensors/edit", options, "Edit")}
            <a href="/sensors" data-link="">Cancel</a>
        `;
    }
}
