import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";
import { getOptionsForSensorType, getSensorTypes } from "./SensorHelpers.js";

export default class extends AbstractView {

    constructor(params) {
        super(params);
    }

    async getHtml() {
        const sensorTypes = await getSensorTypes();
        const sensorType = sensorTypes[this.params.type];
        const sensorOptions = await getOptionsForSensorType(this.params.type);

        sensorOptions.type.value = this.params.type;
        
        this.setTitle(`Add ${sensorType} sensor`);

        return `
            <h1>Add ${sensorType} sensor</h1>
            ${formFromOptions("/api/sensors/add", sensorOptions, "Add")}
        `;
    }
}
