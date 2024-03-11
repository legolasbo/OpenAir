import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";
import { getSensorInfo, getSensorTypes } from "./SensorHelpers.js";

export default class extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle(`Edit sensor: ${this.params.uuid}`);
    }

    async getHtml() {
        const {options, config} = await getSensorInfo(this.params.uuid);
        const sensorTypes = await getSensorTypes();
        const sensorType = sensorTypes[config.type];

        Object.entries(config).forEach(([k,v]) => {
            if (options[k] !== undefined) {
                options[k].value = v;
            }
        })
        
        this.setTitle(`Edit ${sensorType} sensor`);
        
        delete(options.type);
        return `
            <h1>Edit ${sensorType} sensor</h1>
            ${formFromOptions("/api/sensors/edit", options, "Edit")}
            <a href="/sensors" data-link="">Cancel</a>
        `;
    }
}
